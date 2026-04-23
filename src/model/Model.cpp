// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2020, Clément MARCEL.
//
// This file is part of conducteö software.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the  Free Software Foundation,  either version 3 of the License,  or
// (at your option) any later version.
//
// This program is  distributed  in  the  hope that it  will be useful,
// but  WITHOUT ANY WARRANTY ;  without even  the  implied  warranty of
// MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU  General  Public  License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <model/Model.h>
#include <numeric/CouplingCoefficientComputation.h>
#include <databases/BoundaryConditions.h>
#include <physics/AirCavityFromMesh.h>
#include <physics/BoundaryCondition.h>
#include <databases/Environments.h>
#include <geometry/GeometryTools.h>
#include <plugins/En13370Plugin.h>
#include <physics/Environment.h>
#include <errors/ErrorHandler.h>
#include <databases/Materials.h>
#include <model/DxfContent.h>
#include <physics/Material.h>
#include <model/Annotation.h>
#include <model/Rectangle.h>
#include <model/Surface.h>
#include <tools/ToolBox.h>
#include <geometry/Line.h>
#include <numeric/Mesh.h>
#include <numeric/Cell.h>
#include <model/Volume.h>
#include <model/Image.h>
#include <tinyxml.h>
#include <time.h>
#include <cmath>
#include <Log.h>

// Boolean that will be used to abort simulation.
bool g_conducteo_aborted=false;

Model::Model():
    _1dModel(0),
    _mesh(0),
    _running(false),
    _deleteObject(false),
    _psiCoefficientComputed(false),
    _callback(0),
    _caller(0),
    _currentError(0),
    _en13370(0),
    _is1dModel(false),
    _2dModelUserView(false),
    _1dModelUserView(false),
    _resultsUserView(false),
    _envsUserView(false),
    _filetype(ConducteoFile),
    _computationType(ThermalBridge),
    _transmissionCoefficientLength(1.0),
    _transmissionCoefficientLengthType(FullLength),
    _validResults(false),
    _equivalentConductivityWidth(0.0),
    _equivalentConductivityThickness(0.0),
    _equivalentConductivityResistanceSum(0.0),
    _glaserMesh(0),
    _condensation(false),
    _condensationUserView(false)
{
}

Model::Model(const Model &model)
{
    LOG_INFO("Copy model.");

    _mesh=0;
    _glaserMesh = 0;
    _deleteObject=false;
    _running=model._running;
    _callback=model._callback;
    _caller=model._caller;
    _currentError=0;
    _computationType = model._computationType;
    _en13370=0;

    for (unsigned int i=0 ; i<model.std::vector<Surface*>::size() ; i++)
        std::vector<Surface*>::push_back(new Surface(*model.std::vector<Surface*>::at(i)));
    for (unsigned int i=0 ; i<model.std::vector<Volume*>::size() ; i++)
        std::vector<Volume*>::push_back(new Volume(*model.std::vector<Volume*>::at(i)));
    for (unsigned int i=0 ; i<model.std::vector<Image*>::size() ; i++)
        std::vector<Image*>::push_back(new Image(*model.std::vector<Image*>::at(i)));
    for (unsigned int i=0 ; i<model.std::vector<DxfContent*>::size() ; i++)
        std::vector<DxfContent*>::push_back(new DxfContent(*model.std::vector<DxfContent*>::at(i)));
    for (unsigned int i=0 ; i<model.std::vector<Annotation*>::size() ; i++)
        std::vector<Annotation*>::push_back(new Annotation(*model.std::vector<Annotation*>::at(i)));

    // Copy 1D model.
    _1dModel=0;
    if (model._1dModel)
    {
        _1dModel=new Model(*model._1dModel);
        _1dModel->_callback=0;
        _1dModel->_caller=0;
        _1dModel->_is1dModel=true;
        _1dModel->_glaserMesh = 0;
    }
}

Model::~Model()
{
    LOG_INFO("Delete model.");

    clean();

    if (_mesh)
        delete _mesh;

    if (_1dModel)
        delete _1dModel;

    if (_glaserMesh)
        delete _glaserMesh;
    _glaserMesh = 0;

    removeEn13370Plugin();

    LOG_INFO("End of model deletion.");
}

Model *Model::model1d() const
{
    return _1dModel;
}

void Model::clean()
{
    LOG_INFO("Clean model.");

    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        delete std::vector<Volume*>::at(i);
    std::vector<Volume*>::resize(0);

    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
        delete std::vector<Surface*>::at(i);
    std::vector<Surface*>::resize(0);

    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        delete std::vector<Image*>::at(i);
    std::vector<Image*>::resize(0);

    for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
        delete std::vector<DxfContent*>::at(i);
    std::vector<DxfContent*>::resize(0);

    for (unsigned int i=0 ; i<std::vector<Annotation*>::size() ; i++)
        delete std::vector<Annotation*>::at(i);
    std::vector<Annotation*>::resize(0);
}

void Model::fromXml(const std::string &xml, ErrorHandler *error, bool updateSurfaces)
{
    LOG_INFO("Import model from Xml.");

    // Clear material & bc swapping.
    _swap_materials.clear();
    _swap_boundaries.clear();

    // Try importing model from Therm 7.3 importer.
    if (xml.size() > 2)
    {
        unsigned char c0 = (unsigned char) xml.at(0);
        unsigned char c1 = (unsigned char) xml.at(1);

        if (c0 == 0x30 && c1 == 0xbb)
        {
            _filetype = Therm_7_3_File;
            return ThermImporter::loadThermFile(xml);
        }
    }

    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *model=doc.RootElement();
    if (!model)
    {
        setError(error, BadModelFileRootElement);
        return;
    }

    // Read root node.
    const char *root_name=model->Value();
    if (!root_name || std::string(root_name)!="model")
    {
        setError(error, BadModelFileRootElement);
        return;
    }

    // Model file version.
    const char *version=model->Attribute("version");
    if (!version)
    {
        setError(error, BadModelFileVersion);
        return;
    }

    unsigned int v=(unsigned int) ToolBox::convertStringToDouble(version);
    if (v==2)
        fromV2Xml(xml, error);
    else if (v==3)
        fromV3Xml(xml, error);
    else
    {
        setError(error, BadModelFileVersion);
        return;
    }

    // Import environments.
    importEnvironments(xml);

    // Interop import.
    importInterop(xml, error);

    // If import has produced error: clean model.
    if (error && error->code()!=ErrorHandler::NoError)
        return clean();

    // If no error, check for consistency.
    if (updateSurfaces && !check(error))
        return clean();
}

void Model::removeVolume(Volume *v)
{
    LOG_INFO("Remove volume.");

    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        if (volume==v)
        {
            std::vector<Volume*>::erase(std::vector<Volume*>::begin()+i);
            delete v;
            return;
        }
    }
}

void Model::removeVolume(const std::string &id)
{
    LOG_INFO("Remove volume.");

    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        if (volume->id()==id)
        {
            std::vector<Volume*>::erase(std::vector<Volume*>::begin()+i);
            delete volume;
            return;
        }
    }
}

void Model::removeImage(Image *image)
{
    LOG_INFO("Remove image.");

    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
    {
        Image *im=std::vector<Image*>::at(i);
        if (im==image)
        {
            std::vector<Image*>::erase(std::vector<Image*>::begin()+i);
            delete image;
            return;
        }
    }
}

void Model::removeImage(const std::string &id)
{
    LOG_INFO("Remove image.");

    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
    {
        Image *im=std::vector<Image*>::at(i);
        if (im->id()==id)
        {
            std::vector<Image*>::erase(std::vector<Image*>::begin()+i);
            delete im;
            return;
        }
    }
}

void Model::removeDxf(DxfContent *dxf)
{
    LOG_INFO("Remove Dxf.");

    for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *d=std::vector<DxfContent*>::at(i);
        if (d==dxf)
        {
            std::vector<DxfContent*>::erase(std::vector<DxfContent*>::begin()+i);
            delete dxf;
            return;
        }
    }
}

void Model::removeDxf(const std::string &id)
{
    LOG_INFO("Remove Dxf.");

    for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *d=std::vector<DxfContent*>::at(i);
        if (d->id()==id)
        {
            std::vector<DxfContent*>::erase(std::vector<DxfContent*>::begin()+i);
            delete d;
            return;
        }
    }
}

void Model::removeAnnotation(const std::string &id)
{
    LOG_INFO("Remove annotation.");

    for (unsigned int i=0 ; i<std::vector<Annotation*>::size() ; i++)
    {
        Annotation *d=std::vector<Annotation*>::at(i);
        if (d->id()==id)
        {
            std::vector<Annotation*>::erase(std::vector<Annotation*>::begin()+i);
            delete d;
            return;
        }
    }
}

Image *Model::getImageById(const std::string &id) const
{
    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
    {
        Image *im=std::vector<Image*>::at(i);
        if (im->id()==id)
            return im;
    }
    return 0;
}

DxfContent *Model::getDxfById(const std::string &id) const
{
    for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *d=std::vector<DxfContent*>::at(i);
        if (d->id()==id)
            return d;
    }
    return 0;
}

bool Model::check(ErrorHandler *error)
{
    LOG_INFO("Check model.");

    // Recreate surfaces to ensure consistency.
    recreateSurfaces();

    // Check for volumes.
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        if (!volume->check(error))
            return false;
    }

    // Check for surfaces.
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        if (!surface->check(error))
            return false;
    }
    return true;
}

void Model::fromV2Xml(const std::string &xml, ErrorHandler *error)
{
    LOG_INFO("Import v2 model.");

    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *model=doc.RootElement();
    if (!model)
    {
        setError(error, BadModelFileRootElement);
        return;
    }

    // Computation type.
    importComputationType(model);

    // List all materials included into model file.
    std::vector<Material*> materials;
    TiXmlElement *n_materials=model->FirstChildElement("materials");
    if (n_materials)
    {
        TiXmlElement *n_material=n_materials->FirstChildElement("material");
        while (n_material)
        {
            std::string material_s=ToolBox::domToString(n_material);
            if (!material_s.empty())
            {
                Material *m=Material::createFromXml(material_s, error);
                if (m)
                    materials.push_back(m);
            }
                
            n_material=n_material->NextSiblingElement("material");
        }
    }

    // Add all materials if needed.
    std::map<std::string, std::string> swap_materials;
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *m1=materials.at(i);
        m1->setCategory(_materialImportCategory);

        // Get local db material with same id.
        Material *m2=Materials::instance()->materialFromId(m1->id());

        // Material is not yet referenced: add it if no duplicate.
        if (!m2)
        {
            // Get duplicate.
            Material *dup=Materials::instance()->duplicate(m1);

            // No duplicate: add it.
            if (!dup)
            {
                Material *m3=new Material(*m1);
                Materials::instance()->addMaterialToUserDb(m3);
            }

            // Duplicate: store substitution.
            else
            {
                swap_materials[m1->id()]=dup->id();
            }
        }

        // Material already stored into local db.
        else
        {
            // File material is identical to local db material: do nothing.
            // Materials are different: they use the same id. So copy imported
            // material with new id.
            Material *dup=Materials::instance()->duplicate(m1);
            if (!m1->isEquivalentTo(m2) && !dup)
            {
                Material *m3=new Material(*m1);
                m3->setId(ToolBox::getNewUuuid());
                Materials::instance()->addMaterialToUserDb(m3);

                swap_materials[m1->id()]=m3->id();
            }
            else if (dup)
            {
                swap_materials[m1->id()]=dup->id();
            }
        }

        // Delete temporary material.
        delete m1;
    }
    materials.resize(0);

    // List all boundary conditions included into model file.
    std::vector<BoundaryCondition*> boundaries;
    TiXmlElement *n_boundaries=model->FirstChildElement("boundary-conditions");
    if (n_boundaries)
    {
        TiXmlElement *n_boundary=n_boundaries->FirstChildElement("boundary-condition");
        while (n_boundary)
        {
            std::string boundary_s=ToolBox::domToString(n_boundary);
            if (!boundary_s.empty())
            {
                BoundaryCondition *b=BoundaryCondition::createFromXml(boundary_s, error);
                if (b)
                    boundaries.push_back(b);
            }
            n_boundary=n_boundary->NextSiblingElement("boundary-condition");
        }
    }

    // Add all boundary conditions if needed.
    std::map<std::string, std::string> swap_boundaries;
    for (unsigned int i=0 ; i<boundaries.size() ; i++)
    {
        BoundaryCondition *bc1=boundaries.at(i);

        // Get local db boundary condition with same id.
        BoundaryCondition *bc2=BoundaryConditions::instance()->bcFromId(bc1->id());

        // Boundary condition is not yet referenced: add it if no duplicate.
        if (!bc2)
        {
            // Get duplicate.
            BoundaryCondition *dup=BoundaryConditions::instance()->duplicate(bc1);

            // No duplicate: add it.
            if (!dup)
            {
                BoundaryCondition *bc3=new BoundaryCondition(*bc1);
                BoundaryConditions::instance()->addBcToUserDb(bc3);
            }

            // Duplicate: store substitution.
            else
            {
                swap_boundaries[bc1->id()]=dup->id();
            }
        }

        // Boundary condition already stored into local db.
        else
        {
            // File boundary condition is identical to local db boundary condition: do nothing.
            // Boundary conditions are different: they use the same id. So copy imported
            // boundary condition with new id.
            if (!bc1->isEquivalentTo(bc2))
            {
                BoundaryCondition *bc3=new BoundaryCondition(*bc1);
                bc3->setId(ToolBox::getNewUuuid());
                BoundaryConditions::instance()->addBcToUserDb(bc3);

                swap_boundaries[bc1->id()]=bc3->id();
            }
        }

        // Delete temporary material.
        delete bc1;
    }
    boundaries.resize(0);

    // Get "graphics-model" node.
    TiXmlElement *graphics_model=model->FirstChildElement("graphics-model");
    if (!graphics_model)
        return;

    // List all volumes.
    TiXmlElement *volume=graphics_model->FirstChildElement("volume");
    while (volume)
    {
        // Read v2 c2d content for volume.
        Rectangle *rectangle=new Rectangle;
        std::vector<Volume*>::push_back(rectangle);

        // Rectangle origin.
        double x=0.0;
        TiXmlElement *x_n=volume->FirstChildElement("x");
        if (x_n && x_n->GetText())
            x=ToolBox::convertStringToDouble(x_n->GetText());

        double y=0.0;
        TiXmlElement *y_n=volume->FirstChildElement("y");
        if (y_n && y_n->GetText())
            y=-ToolBox::convertStringToDouble(y_n->GetText());

        // Rectangle dimensions.
        double width=0.0;
        TiXmlElement *width_n=volume->FirstChildElement("width");
        if (width_n && width_n->GetText())
            width=ToolBox::convertStringToDouble(width_n->GetText());

        double height=0.0;
        TiXmlElement *height_n=volume->FirstChildElement("height");
        if (height_n && height_n->GetText())
            height=ToolBox::convertStringToDouble(height_n->GetText());

        y-=height;
        rectangle->create(x, y, width, height);

        // Cells.
        TiXmlElement *cells=volume->FirstChildElement("cells");
        if (cells)
        {
            double w=0.025;
            double h=0.025;
            TiXmlElement *cells_width=cells->FirstChildElement("max-width");
            if (cells_width && cells_width->GetText())
                w=ToolBox::convertStringToDouble(cells_width->GetText());

            TiXmlElement *cells_height=cells->FirstChildElement("max-height");
            if (cells_height && cells_height->GetText())
                h=ToolBox::convertStringToDouble(cells_height->GetText());
                
            rectangle->setCellSize(std::min(w, h), error);
        }

        // Material.
        TiXmlElement *conductivity=volume->FirstChildElement("conductivity");
        if (conductivity && conductivity->Attribute("ref"))
        {
            std::string material_ref=conductivity->Attribute("ref");
            Material *m=Materials::instance()->materialFromId(material_ref);

            std::string swap_material=swap_materials[material_ref];
            if (!swap_material.empty())
                m=Materials::instance()->materialFromId(swap_material);

            rectangle->setMaterial(m);
        }

        volume=volume->NextSiblingElement("volume");
    }

    // List all surfaces.
    TiXmlElement *surface=graphics_model->FirstChildElement("surface");
    while (surface)
    {
        // Read v2 c2d content for surface.
        Surface *s=new Surface;
        std::vector<Surface*>::push_back(s);

        // Line.
        TiXmlElement *begin=surface->FirstChildElement("begin");
        TiXmlElement *end=surface->FirstChildElement("end");
        if (begin && end)
        {
            double x0=0.0;
            double y0=0.0;
            TiXmlElement *x_n=begin->FirstChildElement("x");
            if (x_n && x_n->GetText())
                x0=ToolBox::convertStringToDouble(x_n->GetText());

            TiXmlElement *y_n=begin->FirstChildElement("y");
            if (y_n && y_n->GetText())
                y0=-ToolBox::convertStringToDouble(y_n->GetText());

            Point p0;
            p0.setX(x0);
            p0.setY(y0);

            double x1=0.0;
            double y1=0.0;
            x_n=end->FirstChildElement("x");
            if (x_n && x_n->GetText())
                x1=ToolBox::convertStringToDouble(x_n->GetText());

            y_n=end->FirstChildElement("y");
            if (y_n && y_n->GetText())
                y1=-ToolBox::convertStringToDouble(y_n->GetText());

            Point p1;
            p1.setX(x1);
            p1.setY(y1);

            Line *l=new Line;
            l->setBegin(p0, 0);
            l->setEnd(p1, 0);
            s->push_back(l);
        }

        // Boundary condition.
        // Version 2.1.
        TiXmlElement *bc_n=surface->FirstChildElement("boundary-condition");
        if (bc_n && bc_n->GetText())
        {
            std::string bc_ref=bc_n->GetText();
            BoundaryCondition *m=BoundaryConditions::instance()->bcFromId(bc_ref);

            std::string swap_bc=swap_boundaries[bc_ref];
            if (!swap_bc.empty())
                m=BoundaryConditions::instance()->bcFromId(swap_bc);

            s->setBoundaryCondition(m);
        }

        // Version 2.0
        else
        {
            // Read "temperature" & "resistance" nodes.
            TiXmlElement *temperature_n=surface->FirstChildElement("temperature");
            TiXmlElement *resistance_n=surface->FirstChildElement("resistance");
            if (temperature_n && temperature_n->GetText() && resistance_n && resistance_n->GetText())
            {
                double resistance=ToolBox::convertStringToDouble(resistance_n->GetText());
                // If boundary type is adiabatic: do nothing.
                if (resistance>0.0)
                {
                    double temperature=ToolBox::convertStringToDouble(temperature_n->GetText());

                    // Get database boundary condition with previous read temperature & resistance.
                    // If not found: create it.
                    BoundaryCondition *m=BoundaryConditions::instance()->bcFromTemperatureAndResistance(temperature, resistance);

                    if (!m)
                    {
                        std::string name="R="+std::string(resistance_n->GetText())+"m².°C/W - T="+std::string(temperature_n->GetText())+"°C";
                        m=new BoundaryCondition;
                        m->setName(name);
                        m->setTemperature(temperature);
                        m->setResistance(resistance, error);
                        m->setColor((rand()+50)%256, (rand()+50)%256, (rand()+50)%256);
                        BoundaryConditions::instance()->addBcToUserDb(m);
                    }
                    s->setBoundaryCondition(m);
                }
            }
        }

        // Environment.
        TiXmlElement *environment_n=surface->FirstChildElement("environment");
        if (environment_n && environment_n->GetText())
        {
            std::string env_ref=environment_n->GetText();
            Environment *e=Environments::instance()->environmentFromName(env_ref);
            s->setEnvironment(e);
        }

        surface=surface->NextSiblingElement("surface");
    }

    // 1D model.
    TiXmlElement *model_1D=model->FirstChildElement("model-1d");
    if (!_1dModel)
    {
        _1dModel=new Model;
        _1dModel->_is1dModel=true;
    }
    if (model_1D && model_1D->FirstChildElement("model"))
    {
        _1dModel->fromV2Xml(ToolBox::domToString(model_1D->FirstChildElement("model")), error);
    }

    // Load plugins.
    TiXmlElement *plugin=model->FirstChildElement("plugin");
    while(plugin)
    {
        std::string plugin_xml=ToolBox::domToString(plugin);

        const char *type=plugin->Attribute("type");
        if (type)
        {
            std::string s_type=type;

            // EN 13370 plugin.
            if (s_type=="en-13370-plugin")
            {
                removeEn13370Plugin();
                createEn13370Plugin();
                _en13370->fromXml(plugin_xml);
            }
        }
        plugin=plugin->NextSiblingElement("plugin");
    }

    // Force surfaces regeneration.
    recreateSurfaces();
}

void Model::importComputationType(TiXmlElement *model)
{
    if (!model)
        return;

    TiXmlElement *n_computation=model->FirstChildElement("computation");
    if (n_computation)
    {
        const char *c_type=n_computation->Attribute("type");
        if (c_type)
        {
            std::string s_type=c_type;
            if (s_type=="physical-flux")
                _computationType=Flux;
            else if (s_type=="thermal-transmission")
                _computationType=ThermalTransmission;
            else if (s_type=="lambda-equivalent")
                _computationType=EquivalentThermalConductivity;
            else if (s_type=="condensation-glaser")
                _computationType=GlaserCondensation;
        }

        // Thermal transmission.
        TiXmlElement *n_length=n_computation->FirstChildElement("length");
        if (_computationType==ThermalTransmission && n_length)
        {
            const char *c_type=n_length->Attribute("type");
            if (c_type)
            {
                std::string s_type=c_type;
                if (s_type=="full-length")
                    _transmissionCoefficientLengthType=FullLength;
                else if (s_type=="projected-x")
                    _transmissionCoefficientLengthType=ProjectedXLength;
                else if (s_type=="projected-y")
                    _transmissionCoefficientLengthType=ProjectedYLength;
                else if (s_type=="custom-length")
                    _transmissionCoefficientLengthType=CustomLength;
            }

            // Custom length.
            if (_transmissionCoefficientLengthType==CustomLength)
            {
                TiXmlElement *n_value=n_length->FirstChildElement("value");
                const char *c_type=n_value->GetText();
                if (c_type)
                    _transmissionCoefficientLength=ToolBox::stringToDouble(c_type);
            }
        }

        // Lambda equivalent.
        TiXmlElement *n_width = n_computation->FirstChildElement("width");
        TiXmlElement *n_thick = n_computation->FirstChildElement("thickness");
        TiXmlElement *n_res   = n_computation->FirstChildElement("resistances");

        if (_computationType == EquivalentThermalConductivity && n_width && n_thick && n_res)
        {
            const char *c_width = n_width->GetText();
            const char *c_thick = n_thick->GetText();
            const char *c_res   = n_res->GetText();

            if (c_width)
                _equivalentConductivityWidth = ToolBox::stringToDouble(c_width);

            if (c_thick)
                _equivalentConductivityThickness = ToolBox::stringToDouble(c_thick);

            if (c_res)
                _equivalentConductivityResistanceSum = ToolBox::stringToDouble(c_res);
        }
    }
}

void Model::fromV3Xml(const std::string &xml, ErrorHandler *error)
{
    LOG_INFO("Import v3 model.");

    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *model=doc.RootElement();
    if (!model)
    {
        setError(error, BadModelFileRootElement);
        return;
    }

    // Computation type.
    importComputationType(model);

    // List all materials included into model file.
    std::vector<Material*> materials;
    TiXmlElement *n_materials=model->FirstChildElement("materials");
    if (n_materials)
    {
        TiXmlElement *n_material=n_materials->FirstChildElement("material");
        while (n_material)
        {
            std::string material_s=ToolBox::domToString(n_material);
            if (!material_s.empty())
            {
                Material *m=Material::createFromXml(material_s, error);
                if (m)
                {
                    m->setFavorite(false);
                    materials.push_back(m);
                }
            }
                
            n_material=n_material->NextSiblingElement("material");
        }
    }

    // Add all materials if needed.
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *m1=materials.at(i);
        m1->setCategory(_materialImportCategory);

        // Get local db material with same id.
        Material *m2=Materials::instance()->materialFromId(m1->id());

        // Material is not yet referenced: add it if no duplicate.
        if (!m2)
        {
            // Get duplicate.
            Material *dup=Materials::instance()->duplicate(m1);

            // No duplicate: add it.
            if (!dup)
            {
                Material *m3=new Material(*m1);
                Materials::instance()->addMaterialToUserDb(m3);
            }

            // Duplicate: store substitution.
            else
            {
                _swap_materials[m1->id()]=dup->id();
            }
        }

        // Material already stored into local db.
        else
        {
            // File material is identical to local db material: do nothing.
            // Materials are different: they use the same id. So copy imported
            // material with new id.
            Material *dup=Materials::instance()->duplicate(m1);
            if (!m1->isEquivalentTo(m2) && m1->id()!="106D6720-211C-4090-8C60-7CED671C8580" && m1->id()!="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B" && !dup)
            {
                Material *m3=new Material(*m1);
                m3->setId(ToolBox::getNewUuuid());
                Materials::instance()->addMaterialToUserDb(m3);

                _swap_materials[m1->id()]=m3->id();
            }
            else if (dup && m1->id()!="106D6720-211C-4090-8C60-7CED671C8580" && m1->id()!="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
            {
                _swap_materials[m1->id()]=dup->id();
            }
        }

        // Delete temporary material.
        delete m1;
    }
    materials.resize(0);

    // List all boundary conditions included into model file.
    std::vector<BoundaryCondition*> boundaries;
    TiXmlElement *n_boundaries=model->FirstChildElement("boundary-conditions");
    if (n_boundaries)
    {
        TiXmlElement *n_boundary=n_boundaries->FirstChildElement("boundary-condition");
        while (n_boundary)
        {
            std::string boundary_s=ToolBox::domToString(n_boundary);
            if (!boundary_s.empty())
            {
                BoundaryCondition *bc=BoundaryCondition::createFromXml(boundary_s, error);
                if (bc)
                    boundaries.push_back(bc);
            }
            n_boundary=n_boundary->NextSiblingElement("boundary-condition");
        }
    }

    // Add all boundary conditions if needed.
    for (unsigned int i=0 ; i<boundaries.size() ; i++)
    {
        BoundaryCondition *bc1=boundaries.at(i);

        // Get local db boundary condition with same id.
        BoundaryCondition *bc2=BoundaryConditions::instance()->bcFromId(bc1->id());

        // Boundary condition is not yet referenced: add it if no duplicate.
        if (!bc2)
        {
            // Get duplicate.
            BoundaryCondition *dup=BoundaryConditions::instance()->duplicate(bc1);

            // No duplicate: add it.
            if (!dup)
            {
                BoundaryCondition *bc3=new BoundaryCondition(*bc1);
                BoundaryConditions::instance()->addBcToUserDb(bc3);
            }

            // Duplicate: store substitution.
            else
            {
                _swap_boundaries[bc1->id()]=dup->id();
            }
        }

        // Boundary condition already stored into local db.
        else
        {
            // File boundary condition is identical to local db boundary condition: do nothing.
            // Boundary conditions are different: they use the same id. So copy imported
            // boundary condition with new id.
            if (!bc1->isEquivalentTo(bc2))
            {
                BoundaryCondition *bc3=new BoundaryCondition(*bc1);
                bc3->setId(ToolBox::getNewUuuid());
                BoundaryConditions::instance()->addBcToUserDb(bc3);

                _swap_boundaries[bc1->id()]=bc3->id();
            }
        }

        // Delete temporary material.
        delete bc1;
    }
    boundaries.resize(0);

    // List all volumes.
    TiXmlElement *volume=model->FirstChildElement("volume");
    while (volume)
    {
        std::string volume_s=ToolBox::domToString(volume);
        if (!volume_s.empty())
            std::vector<Volume*>::push_back(Volume::createFromXml(volume_s, error, _swap_materials));
        volume=volume->NextSiblingElement("volume");
    }

    // List all surfaces.
    TiXmlElement *surface=model->FirstChildElement("surface");
    while (surface)
    {
        std::string surface_s=ToolBox::domToString(surface);
        if (!surface_s.empty())
            std::vector<Surface*>::push_back(Surface::createFromXml(surface_s, error, _swap_boundaries));
        surface=surface->NextSiblingElement("surface");
    }

    // List all images.
    TiXmlElement *image=model->FirstChildElement("image");
    while (image)
    {
        std::string image_s=ToolBox::domToString(image);
        if (!image_s.empty())
            std::vector<Image*>::push_back(Image::createFromXml(image_s, error));
        image=image->NextSiblingElement("image");
    }

    // List all dxf.
    TiXmlElement *dxf=model->FirstChildElement("dxf-import");
    while (dxf)
    {
        std::string dxf_s=ToolBox::domToString(dxf);
        if (!dxf_s.empty())
            std::vector<DxfContent*>::push_back(DxfContent::createFromXml(dxf_s, error));
        dxf=dxf->NextSiblingElement("dxf-import");
    }

    // List all annotations.
    TiXmlElement *n_annotations=model->FirstChildElement("annotations");
    if (n_annotations)
    {
        TiXmlElement *n_annotation=n_annotations->FirstChildElement("annotation");
        while (n_annotation)
        {
            std::string annotation_s=ToolBox::domToString(n_annotation);
            if (!annotation_s.empty())
            {
                Annotation *a=new Annotation;
                a->importFromXml(annotation_s);
                std::vector<Annotation*>::push_back(a);
            }
                
            n_annotation=n_annotation->NextSiblingElement("annotation");
        }
    }

    // Load plugins.
    TiXmlElement *plugin=model->FirstChildElement("plugin");
    while(plugin)
    {
        std::string plugin_xml=ToolBox::domToString(plugin);

        const char *type=plugin->Attribute("type");
        if (type)
        {
            std::string s_type=type;

            // EN 13370 plugin.
            if (s_type=="en-13370-plugin")
            {
                removeEn13370Plugin();
                createEn13370Plugin();
                _en13370->fromXml(plugin_xml);
            }
        }
        plugin=plugin->NextSiblingElement("plugin");
    }
    
    // User-defined view zooms.
    _1dModelUserView=false;
    TiXmlElement *model_1D_zoom=model->FirstChildElement("view-zoom-1d-model");
    if (model_1D_zoom)
    {
        TiXmlElement *xmin=model_1D_zoom->FirstChildElement("x-min");
        TiXmlElement *ymin=model_1D_zoom->FirstChildElement("y-min");
        TiXmlElement *xmax=model_1D_zoom->FirstChildElement("x-max");
        TiXmlElement *ymax=model_1D_zoom->FirstChildElement("y-max");
        
        if (xmin && xmin->GetText() && ymin && ymin->GetText() && xmax && xmax->GetText() && ymax && ymax->GetText())
        {
            std::string s_xmin=xmin->GetText();
            std::string s_ymin=ymin->GetText();
            std::string s_xmax=xmax->GetText();
            std::string s_ymax=ymax->GetText();

            _1dModelXmin=ToolBox::stringToDouble(s_xmin);
            _1dModelYmin=ToolBox::stringToDouble(s_ymin);
            _1dModelXmax=ToolBox::stringToDouble(s_xmax);
            _1dModelYmax=ToolBox::stringToDouble(s_ymax);
            _1dModelUserView=true;
        }
    }

    _2dModelUserView=false;
    TiXmlElement *model_2D_zoom=model->FirstChildElement("view-zoom-2d-model");
    if (model_2D_zoom)
    {
        TiXmlElement *xmin=model_2D_zoom->FirstChildElement("x-min");
        TiXmlElement *ymin=model_2D_zoom->FirstChildElement("y-min");
        TiXmlElement *xmax=model_2D_zoom->FirstChildElement("x-max");
        TiXmlElement *ymax=model_2D_zoom->FirstChildElement("y-max");
        
        if (xmin && xmin->GetText() && ymin && ymin->GetText() && xmax && xmax->GetText() && ymax && ymax->GetText())
        {
            std::string s_xmin=xmin->GetText();
            std::string s_ymin=ymin->GetText();
            std::string s_xmax=xmax->GetText();
            std::string s_ymax=ymax->GetText();

            _2dModelXmin=ToolBox::stringToDouble(s_xmin);
            _2dModelYmin=ToolBox::stringToDouble(s_ymin);
            _2dModelXmax=ToolBox::stringToDouble(s_xmax);
            _2dModelYmax=ToolBox::stringToDouble(s_ymax);
            _2dModelUserView=true;
        }
    }

    _resultsUserView=false;
    TiXmlElement *results_zoom=model->FirstChildElement("view-zoom-results");
    if (results_zoom)
    {
        TiXmlElement *xmin=results_zoom->FirstChildElement("x-min");
        TiXmlElement *ymin=results_zoom->FirstChildElement("y-min");
        TiXmlElement *xmax=results_zoom->FirstChildElement("x-max");
        TiXmlElement *ymax=results_zoom->FirstChildElement("y-max");
        
        if (xmin && xmin->GetText() && ymin && ymin->GetText() && xmax && xmax->GetText() && ymax && ymax->GetText())
        {
            std::string s_xmin=xmin->GetText();
            std::string s_ymin=ymin->GetText();
            std::string s_xmax=xmax->GetText();
            std::string s_ymax=ymax->GetText();

            _resultsXmin=ToolBox::stringToDouble(s_xmin);
            _resultsYmin=ToolBox::stringToDouble(s_ymin);
            _resultsXmax=ToolBox::stringToDouble(s_xmax);
            _resultsYmax=ToolBox::stringToDouble(s_ymax);
            _resultsUserView=true;
        }
    }

    _envsUserView=false;
    TiXmlElement *envs_zoom=model->FirstChildElement("view-zoom-environments");
    if (envs_zoom)
    {
        TiXmlElement *xmin=envs_zoom->FirstChildElement("x-min");
        TiXmlElement *ymin=envs_zoom->FirstChildElement("y-min");
        TiXmlElement *xmax=envs_zoom->FirstChildElement("x-max");
        TiXmlElement *ymax=envs_zoom->FirstChildElement("y-max");

        if (xmin && xmin->GetText() && ymin && ymin->GetText() && xmax && xmax->GetText() && ymax && ymax->GetText())
        {
            std::string s_xmin=xmin->GetText();
            std::string s_ymin=ymin->GetText();
            std::string s_xmax=xmax->GetText();
            std::string s_ymax=ymax->GetText();

            _envsXmin=ToolBox::stringToDouble(s_xmin);
            _envsYmin=ToolBox::stringToDouble(s_ymin);
            _envsXmax=ToolBox::stringToDouble(s_xmax);
            _envsYmax=ToolBox::stringToDouble(s_ymax);
            _envsUserView=true;
        }
    }

    _condensationUserView = false;
    TiXmlElement *condensation_zoom=model->FirstChildElement("view-zoom-condensation");
    if (condensation_zoom)
    {
        TiXmlElement *xmin=condensation_zoom->FirstChildElement("x-min");
        TiXmlElement *ymin=condensation_zoom->FirstChildElement("y-min");
        TiXmlElement *xmax=condensation_zoom->FirstChildElement("x-max");
        TiXmlElement *ymax=condensation_zoom->FirstChildElement("y-max");

        if (xmin && xmin->GetText() && ymin && ymin->GetText() && xmax && xmax->GetText() && ymax && ymax->GetText())
        {
            std::string s_xmin=xmin->GetText();
            std::string s_ymin=ymin->GetText();
            std::string s_xmax=xmax->GetText();
            std::string s_ymax=ymax->GetText();

            _condensationXmin=ToolBox::stringToDouble(s_xmin);
            _condensationYmin=ToolBox::stringToDouble(s_ymin);
            _condensationXmax=ToolBox::stringToDouble(s_xmax);
            _condensationYmax=ToolBox::stringToDouble(s_ymax);
            _condensationUserView=true;
        }
    }

    // 1D model.
    TiXmlElement *model_1D=model->FirstChildElement("model-1d");
    if (!_1dModel)
    {
        _1dModel=new Model;
        _1dModel->_is1dModel=true;
    }
    if (model_1D && model_1D->FirstChildElement("model") && (_computationType!=Flux) && (_computationType != GlaserCondensation))
    {
        // Copie swapping tables.
        _1dModel->_swap_boundaries = _swap_boundaries;
        _1dModel->_swap_materials  = _swap_materials;

        _1dModel->fromV3Xml(ToolBox::domToString(model_1D->FirstChildElement("model")), error);
    }

    // Force surfaces regeneration.
    recreateSurfaces();
}

std::string Model::exportComputationType() const
{
    std::string type="thermal-bridge";

    if (_computationType==Flux)
        type="physical-flux";
    else if (_computationType==ThermalTransmission)
        type="thermal-transmission";
    else if (_computationType==EquivalentThermalConductivity)
        type="lambda-equivalent";
    else if (_computationType == GlaserCondensation)
        type="condensation-glaser";

    std::string xml = "<computation type=\"" + type + "\">";

    // Thermal transmission case.
    if (_computationType==ThermalTransmission)
    {
        type="full-length";
        if (_transmissionCoefficientLengthType==ProjectedXLength)
            type="projected-x";
        else if (_transmissionCoefficientLengthType==ProjectedYLength)
            type="projected-y";
        else if (_transmissionCoefficientLengthType==CustomLength)
            type="custom-length";
        xml+="<length type=\""+type+"\">";

        // Custom length.
        if (_transmissionCoefficientLengthType==CustomLength)
        {
            xml+="<value>"+ToolBox::doubleToString(_transmissionCoefficientLength)+"</value>";
        }
        xml+="</length>";
    }

    // Lambda equivalent.
    else if (_computationType == EquivalentThermalConductivity)
    {
        xml += "<width>" + ToolBox::doubleToString(_equivalentConductivityWidth) + "</width>";
        xml += "<thickness>" + ToolBox::doubleToString(_equivalentConductivityThickness) + "</thickness>";
        xml += "<resistances>" + ToolBox::doubleToString(_equivalentConductivityResistanceSum) + "</resistances>";
    }

    xml+="</computation>";

    return xml;
}

std::string Model::toXml_V3(ErrorHandler *error, bool export_databases) const
{
    LOG_INFO("Export v3 model.");

    std::string xml="<model version=\"3.0\">";

    if (!_is1dModel)
    {
        xml += exportComputationType();

        // Export environments.
        xml += exportEnvironments();
    }

    std::vector<Material*> materials;
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        xml+=volume->serialize();
        if (volume->material())
            materials.push_back(volume->material());
    }
    if (_1dModel && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        for (unsigned int i=0 ; i<_1dModel->std::vector<Volume*>::size() ; i++)
        {
            Volume *volume=_1dModel->std::vector<Volume*>::at(i);
            if (volume->material())
                materials.push_back(volume->material());
        }
    }
    ToolBox::uniquify<Material*>(materials);

    std::vector<BoundaryCondition*> boundaries;
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        xml+=surface->serialize();
        if (surface->boundaryCondition() && !surface->boundaryCondition()->id().empty())
            boundaries.push_back(surface->boundaryCondition());
    }
    if (_1dModel && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        for (unsigned int i=0 ; i<_1dModel->std::vector<Surface*>::size() ; i++)
        {
            Surface *surface=_1dModel->std::vector<Surface*>::at(i);
            if (surface->boundaryCondition())
                boundaries.push_back(surface->boundaryCondition());
        }
    }
    ToolBox::uniquify<BoundaryCondition*>(boundaries);

    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
    {
        Image *image=std::vector<Image*>::at(i);
        xml+=image->serialize();
    }

    for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *dxf=std::vector<DxfContent*>::at(i);
        xml+=dxf->serialize();
    }

    if (export_databases)
    {
        xml+="<materials>";
        for (unsigned int i=0 ; i<materials.size() ; i++)
            xml+=materials.at(i)->serialize();
        xml+="</materials>";

        xml+="<boundary-conditions>";
        for (unsigned int i=0 ; i<boundaries.size() ; i++)
            xml+=boundaries.at(i)->serialize();
        xml+="</boundary-conditions>";
    }

    // EN 13370.
    if (_en13370)
        xml+=_en13370->toXml();

    // User defined view zooms.
    if (_1dModelUserView && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        xml+="<view-zoom-1d-model>";
        xml+="<x-min>"+ToolBox::doubleToString(_1dModelXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_1dModelYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_1dModelXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_1dModelYmax)+"</y-max>";
        xml+="</view-zoom-1d-model>";
    }

    if (_2dModelUserView)
    {
        xml+="<view-zoom-2d-model>";
        xml+="<x-min>"+ToolBox::doubleToString(_2dModelXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_2dModelYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_2dModelXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_2dModelYmax)+"</y-max>";
        xml+="</view-zoom-2d-model>";
    }

    if (_resultsUserView)
    {
        xml+="<view-zoom-results>";
        xml+="<x-min>"+ToolBox::doubleToString(_resultsXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_resultsYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_resultsXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_resultsYmax)+"</y-max>";
        xml+="</view-zoom-results>";
    }

    if (_envsUserView)
    {
        xml+="<view-zoom-environments>";
        xml+="<x-min>"+ToolBox::doubleToString(_envsXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_envsYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_envsXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_envsYmax)+"</y-max>";
        xml+="</view-zoom-environments>";
    }

    if (_condensationUserView)
    {
        xml+="<view-zoom-condensation>";
        xml+="<x-min>"+ToolBox::doubleToString(_condensationXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_condensationYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_condensationXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_condensationYmax)+"</y-max>";
        xml+="</view-zoom-condensation>";
    }

    // Annotations.
    xml+="<annotations>";
    for (unsigned int i=0 ; i<std::vector<Annotation*>::size() ; i++)
        xml+=std::vector<Annotation*>::at(i)->serialize();
    xml+="</annotations>";

    // 1D model.
    if (_1dModel && !_is1dModel && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        xml+="<model-1d>";
        xml+=_1dModel->toXml_V3(error, false);
        xml+="</model-1d>";
    }

    // Interop export.
    xml+=exportInterop();

    xml+="</model>";
    return xml;
}

std::string Model::toXml_V2(ErrorHandler *error, bool export_databases) const
{
    LOG_INFO("Export v2 model.");

    std::string xml="<model version=\"2.0\">";

    if (!_is1dModel)
    {
        xml += exportComputationType();

        // Export environments.
        xml += exportEnvironments();
    }

    std::vector<Material*> materials;

    // All volumes.
    xml+="<graphics-model>\n";
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        xml+=volume->serializeAsRectangle();
        if (volume->material())
            materials.push_back(volume->material());
    }

    if (_1dModel && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        for (unsigned int i=0 ; i<_1dModel->std::vector<Volume*>::size() ; i++)
        {
            Volume *volume=_1dModel->std::vector<Volume*>::at(i);
            if (volume->material())
                materials.push_back(volume->material());
        }
    }
    ToolBox::uniquify<Material*>(materials);

    std::vector<BoundaryCondition*> boundaries;

    // All surfaces.
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        xml+=surface->serializeAsV2Surface();
        if (surface->boundaryCondition() && !surface->boundaryCondition()->id().empty())
            boundaries.push_back(surface->boundaryCondition());
    }

    if (_1dModel && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        for (unsigned int i=0 ; i<_1dModel->std::vector<Surface*>::size() ; i++)
        {
            Surface *surface=_1dModel->std::vector<Surface*>::at(i);
            if (surface->boundaryCondition())
                boundaries.push_back(surface->boundaryCondition());
        }
    }
    ToolBox::uniquify<BoundaryCondition*>(boundaries);

    xml+="</graphics-model>\n";

    if (export_databases)
    {
        xml+="<materials>";
        for (unsigned int i=0 ; i<materials.size() ; i++)
            xml+=materials.at(i)->serialize();
        xml+="</materials>";

        xml+="<boundary-conditions>";
        for (unsigned int i=0 ; i<boundaries.size() ; i++)
            xml+=boundaries.at(i)->serialize();
        xml+="</boundary-conditions>";
    }

    // EN 13370.
    if (_en13370)
        xml+=_en13370->toXml();

    // User defined view zooms.
    if (_1dModelUserView && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        xml+="<view-zoom-1d-model>";
        xml+="<x-min>"+ToolBox::doubleToString(_1dModelXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_1dModelYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_1dModelXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_1dModelYmax)+"</y-max>";
        xml+="</view-zoom-1d-model>";
    }

    if (_2dModelUserView)
    {
        xml+="<view-zoom-2d-model>";
        xml+="<x-min>"+ToolBox::doubleToString(_2dModelXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_2dModelYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_2dModelXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_2dModelYmax)+"</y-max>";
        xml+="</view-zoom-2d-model>";
    }

    if (_resultsUserView)
    {
        xml+="<view-zoom-results>";
        xml+="<x-min>"+ToolBox::doubleToString(_resultsXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_resultsYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_resultsXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_resultsYmax)+"</y-max>";
        xml+="</view-zoom-results>";
    }

    if (_envsUserView)
    {
        xml+="<view-zoom-environments>";
        xml+="<x-min>"+ToolBox::doubleToString(_envsXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_envsYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_envsXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_envsYmax)+"</y-max>";
        xml+="</view-zoom-environments>";
    }

    if (_condensationUserView)
    {
        xml+="<view-zoom-condensation>";
        xml+="<x-min>"+ToolBox::doubleToString(_condensationXmin)+"</x-min>";
        xml+="<y-min>"+ToolBox::doubleToString(_condensationYmin)+"</y-min>";
        xml+="<x-max>"+ToolBox::doubleToString(_condensationXmax)+"</x-max>";
        xml+="<y-max>"+ToolBox::doubleToString(_condensationYmax)+"</y-max>";
        xml+="</view-zoom-condensation>";
    }

    // 1D model.
    if (_1dModel && !_is1dModel && (_computationType!=Flux) && (_computationType != EquivalentThermalConductivity) && (_computationType != GlaserCondensation))
    {
        xml+="<model-1d>";
        xml+=_1dModel->toXml_V2(error, false);
        xml+="</model-1d>";
    }

    // Interop export.
    xml+=exportInterop();

    xml+="</model>";
    return xml;
}

void Model::addVolume(Volume *volume)
{
    std::vector<Volume*>::push_back(volume);
    recreateSurfaces(volume);
}

void Model::insertVolume(Volume *volume, int position)
{
    if (position<0)
        return;
    std::vector<Volume*>::insert(std::vector<Volume*>::begin()+position, volume);
    recreateSurfaces(volume);
}

void Model::insertImage(Image *image, int position)
{
    if (position<0)
        return;
    std::vector<Image*>::insert(std::vector<Image*>::begin()+position, image);
}

void Model::recreateSurfaces(Volume *volume)
{
    LOG_INFO("Recreate surfaces.");

    // Create all surfaces.
    std::vector<Line*> surfaces=extractBoundarySurfaces(volume);

    // Store old surfaces.
    std::vector<Surface*> old_surfaces=*this;

    // Add new surfaces.
    std::vector<Surface*>::clear();
    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Line *line=surfaces.at(i);
        Surface *surface=new Surface;
        surface->push_back(line);
        std::vector<Surface*>::push_back(surface);

        // Look for old surface data.
        Point middle=line->middlePoint();

        Surface *old_surface=surfaceByPoint(old_surfaces, middle);
        if (!old_surface)
            continue;

        surface->setBoundaryCondition(old_surface->boundaryCondition());
        surface->setEnvironment(old_surface->environment());
    }

    // Delete old surfaces.
    for (unsigned int i=0 ; i<old_surfaces.size() ; i++)
        delete old_surfaces.at(i);

    // Merge surfaces.
    while(mergeSurfaces())
    {
    }

    recreatePointsList();
}

bool Model::mergeSurfaces()
{
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface1=std::vector<Surface*>::at(i);
        if (surface1->empty())
            continue;

        for (unsigned int j=0 ; j<std::vector<Surface*>::size() ; j++)
        {
            if (i!=j)
            {
                Surface *surface2=std::vector<Surface*>::at(j);
                if (surface2->empty())
                    continue;

                // Same initial path.
                Path *s1_ip=surface1->at(0)->initialPath();
                Path *s2_ip=surface2->at(0)->initialPath();

                if (!s1_ip)
                    continue;
                if (!s2_ip)
                    continue;

                if (s1_ip!=s2_ip)
                    continue;

                // Both surfaces have a common point.
                if (!surface1->haveCommonPoints(surface2))
                    continue;

                // Merge two surfaces.
                for (unsigned int k=0 ; k<surface2->size() ; k++)
                {
                    Line *line=surface2->at(k);
                    Line *copy=new Line(*line);
                    surface1->push_back(copy);
                }
                delete surface2;
                std::vector<Surface*>::erase(std::vector<Surface*>::begin()+j);
                
                return true;
            }
        }
    }
    return false;
}

void Model::recreateSurfaces()
{
    recreateSurfaces(0);
}

Surface *Model::surfaceByPoint(const std::vector<Surface*> &surfaces, const Point &point) const
{
    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Surface *surface=surfaces.at(i);

        for (unsigned int j=0 ; j<surface->size() ; j++)
        {
            Line *line=surface->at(j);
            if (line->containsPoint(point))
                return surface;
        }
    }
    return 0;
}

std::vector<Line*> Model::getAllSurfaces() const
{
    std::vector<Line*> lines;

    // Create surface list from volumes paths.
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        std::vector<Line*> s=volume->getLines();
        for (unsigned int j=0 ; j<s.size() ; j++)
            lines.push_back(s.at(j));
    }
    return lines;
}

std::vector<Line*> Model::copySurfacesAndAddVolume(Volume *volume) const
{
    std::vector<Line*> lines;

    // Copy surfaces.
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);

        for (unsigned int j=0 ; j<surface->size() ; j++)
        {
            Line *line=surface->at(j);
            Line *line2=new Line(*line);
            lines.push_back(line2);
        }
    }

    // Add volume lines.
    std::vector<Line*> s=volume->getLines();
    for (unsigned int j=0 ; j<s.size() ; j++)
        lines.push_back(s.at(j));
    return lines;
}

std::vector<Line*> Model::extractBoundarySurfaces(Volume *volume) const
{
    std::vector<Line*> lines;
    // No volume provided: get all surfaces.
    if (!volume)
        lines=getAllSurfaces();

    // Volume provided: copy all existing surfaces & current volume surfaces.
    else
        lines=copySurfacesAndAddVolume(volume);

    std::vector<Line*> all_surfaces=extractNonIntersectedSurfaces(lines);

    return removeInternalSurfaces(all_surfaces);
}

std::vector<Line*> Model::cellAndVolumesSurfaces(Cell *cell, Volume *ref_volume) const
{
    // Get all volumes surfaces.
    std::vector<Line*> lines=getAllSurfaces();

    // Add cell surfaces.
    for (unsigned int i=0 ; i<cell->size() ; i++)
    {
        Line *line=cell->at(i);
        Line *copy=new Line(*line);
        lines.push_back(copy);
    }

    // Extract non intersected surfaces.
    std::vector<Line*> surfaces=extractNonIntersectedSurfaces(lines);

    // Delete all surface with middle point external to cell.
    // Delete all surface with middle point external to reference volume.
    std::vector<Line*> s;
    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Line *line=surfaces.at(i);
        Point m=line->middlePoint();

        bool delete_line=false;
        if (!cell->squareMeshContainsPoint(m))
            delete_line=true;

        if (!delete_line && !ref_volume->containsPoint(m))
            delete_line=true;

        if (delete_line)
            s.push_back(line);
        else
            delete line;
    }

    return s;
}

std::vector<Line*> Model::extractNonIntersectedSurfaces(const std::vector<Line*> &lines) const
{
    // Split into intersected or non-intersected surfaces.
    std::vector<Line*> S2;
    std::vector<Line*> S3;
    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        Line *line1=lines.at(i);
        bool intersection=false;
        for (unsigned int j=0 ; j<lines.size() ; j++)
        {
            if (i!=j)
            {
                Line *line2=lines.at(j);
                if (GeometryTools::intersects(line1, line2))
                {
                    intersection=true;
                    break;
                }
            }
        }

        if (!intersection)
            S2.push_back(line1);
        else
            S3.push_back(line1);
    }

    // Cut all intersected surfaces.
    for (unsigned int i=0 ; i<S3.size() ; i++)
    {
        Line *s=S3.at(i);
        if (!s)
            continue;

        for (unsigned int j=i ; j<S3.size() ; j++)
        {
            Line *sp=S3.at(j);
            if (i!=j && sp)
            {
                // Get intersection points.
                std::vector<Point> intersections=GeometryTools::intersectionPointWithoutCorner(s, sp);
                if (intersections.size()==1)
                {
                    // Cut lines.
                    std::vector<Line*> paths1=s->cut(intersections.at(0));
                    std::vector<Line*> paths2=sp->cut(intersections.at(0));

                    // Create new surfaces.
                    std::vector<Line*> temp_surfaces;
                    for (unsigned int k=0 ; k<paths1.size() ; k++)
                    {
                        Line *line=paths1.at(k);
                        line->setInitialPath(s->initialPath());
                        temp_surfaces.push_back(line);
                    }
                    for (unsigned int k=0 ; k<paths2.size() ; k++)
                    {
                        Line *line=paths2.at(k);
                        line->setInitialPath(sp->initialPath());
                        temp_surfaces.push_back(line);
                    }

                    // Delete old surfaces.
                    delete s;
                    delete sp;
                    S3.at(i)=0;
                    S3.at(j)=0;

                    // Add surfaces that are not already defined.
                    for (unsigned int k=0 ; k<temp_surfaces.size() ; k++)
                    {
                        Line *s=temp_surfaces.at(k);
                        for (unsigned int r=0 ; r<S3.size() ; r++)
                        {
                            Line *t=S3.at(r);
                            if (!t)
                                continue;

                            if (t->containsPath(s))
                            {
                                delete s;
                                s=0;
                                break;
                            }
                        }

                        if (s)
                            S3.push_back(s);
                    }
                    break;
                }
            }
        }
    }

    // Add not-null surfaces from S3 into S2.
    for (unsigned int i=0 ; i<S3.size() ; i++)
    {
        Line *s=S3.at(i);
        if (s)
            S2.push_back(s);
    }

    return S2;
}

std::vector<Line*> Model::removeInternalSurfaces(const std::vector<Line*> &surfaces) const
{
    // Remove internal surfaces.
    // Considering middle point of path wrt all volumes.
    // Remove surfaces that split two volumes.
    std::vector<Line*> lines;
    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Line *line=surfaces.at(i);

        Point middle=line->middlePoint();

        Point near1=line->nearMiddlePoint(true);
        Point near2=line->nearMiddlePoint(false);

        Volume *volume_right=getVolumeAtPosition(near1.x(), near1.y());
        Volume *volume_left=getVolumeAtPosition(near2.x(), near2.y());

        bool volume_on_right=true;
        if (!volume_right)
            volume_on_right=false;
        if (volume_right && !volume_right->material())
            volume_on_right=false;

        bool volume_on_left=true;
        if (!volume_left)
            volume_on_left=false;
        if (volume_left && !volume_left->material())
            volume_on_left=false;

        bool keep_surface=true;
        if (!volume_on_right && !volume_on_left)
            keep_surface=false;
        if (volume_on_right && volume_on_left)
            keep_surface=false;
        
        // Check for surface already defined.
        for (unsigned int j=0 ; j<lines.size() ; j++)
            if (lines.at(j)->isEquivalentTo(line))
                keep_surface=false;

        if (!keep_surface)
            delete line;
        else
            lines.push_back(line);
    }

    return lines;
}

int Model::volumeStackPosition(Volume *volume) const
{
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        if (std::vector<Volume*>::at(i)==volume)
            return i;
    return -1;
}

int Model::volumeStackPosition(const std::string &id) const
{
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        if (std::vector<Volume*>::at(i)->id()==id)
            return i;
    return -1;
}

int Model::imageStackPosition(Image *image) const
{
    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        if (std::vector<Image*>::at(i)==image)
            return i;
    return -1;
}

double Model::xMin(bool showImages, bool showDxf, bool showAnnotations) const
{
    bool defined=false;
    double v=0.0;
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        if (std::vector<Volume*>::at(i)->xMin()<v || !defined)
        {
            v=std::vector<Volume*>::at(i)->xMin();
            defined=true;
        }
    }

    if (showImages)
    {
        for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        {
            if (std::vector<Image*>::at(i)->x()<v || !defined)
            {
                v=std::vector<Image*>::at(i)->x();
                defined=true;
            }
        }
    }

    if (showDxf)
    {
        for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
        {
            if (std::vector<DxfContent*>::at(i)->xMin()<v || !defined)
            {
                v=std::vector<DxfContent*>::at(i)->xMin();
                defined=true;
            }
        }
    }

    return v;
}

double Model::xMax(bool showImages, bool showDxf, bool showAnnotations) const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        if (std::vector<Volume*>::at(i)->xMax()>v || !defined)
        {
            v=std::vector<Volume*>::at(i)->xMax();
            defined=true;
        }
    }

    if (showImages)
    {
        for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        {
            if (std::vector<Image*>::at(i)->x() + std::vector<Image*>::at(i)->width()>v || !defined)
            {
                v=std::vector<Image*>::at(i)->x() + std::vector<Image*>::at(i)->width();
                defined=true;
            }
        }
    }

    if (showDxf)
    {
        for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
        {
            if (std::vector<DxfContent*>::at(i)->xMax()>v || !defined)
            {
                v=std::vector<DxfContent*>::at(i)->xMax();
                defined=true;
            }
        }
    }

    return v;
}

double Model::yMin(bool showImages, bool showDxf, bool showAnnotations) const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        if (std::vector<Volume*>::at(i)->yMin()<v || !defined)
        {
            v=std::vector<Volume*>::at(i)->yMin();
            defined=true;
        }
    }

    if (showImages)
    {
        for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        {
            if (std::vector<Image*>::at(i)->y()<v || !defined)
            {
                v=std::vector<Image*>::at(i)->y();
                defined=true;
            }
        }
    }

    if (showDxf)
    {
        for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
        {
            if (std::vector<DxfContent*>::at(i)->yMin()<v || !defined)
            {
                v=std::vector<DxfContent*>::at(i)->yMin();
                defined=true;
            }
        }
    }

    return v;
}

double Model::yMax(bool showImages, bool showDxf, bool showAnnotations) const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        if (std::vector<Volume*>::at(i)->yMax()>v || !defined)
        {
            v=std::vector<Volume*>::at(i)->yMax();
            defined=true;
        }
    }

    if (showImages)
    {
        for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        {
            if (std::vector<Image*>::at(i)->y() + std::vector<Image*>::at(i)->height()>v || !defined)
            {
                v=std::vector<Image*>::at(i)->y() + std::vector<Image*>::at(i)->height();
                defined=true;
            }
        }
    }

    if (showDxf)
    {
        for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
        {
            if (std::vector<DxfContent*>::at(i)->yMax()>v || !defined)
            {
                v=std::vector<DxfContent*>::at(i)->yMax();
                defined=true;
            }
        }
    }

    return v;
}

Volume *Model::getVolumeAtPosition(double x, double y) const
{
    Point p;
    p.setX(x);
    p.setY(y);
    for (int i=std::vector<Volume*>::size()-1 ; i>=0 ; i--)
    {
        if (std::vector<Volume*>::at(i)->containsPoint(p))
            return std::vector<Volume*>::at(i);
    }
    return 0;
}

Volume *Model::getVolumeById(const std::string &id) const
{
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        if (std::vector<Volume*>::at(i)->id()==id)
            return std::vector<Volume*>::at(i);
    }
    return 0;
}

Surface *Model::getSurfaceAtPosition(double x, double y, double precision) const
{
    Point p;
    p.setX(x);
    p.setY(y);
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        if (std::vector<Surface*>::at(i)->pointDistance(p)<precision)
            return std::vector<Surface*>::at(i);
    }
    return 0;
}

Annotation *Model::getAnnotationAtPosition(double x, double y, double precision) const
{
    Point p;
    p.setX(x);
    p.setY(y);

    for (unsigned int i=0 ; i<std::vector<Annotation*>::size() ; i++)
    {
        if (std::vector<Annotation*>::at(i)->contains(p, precision))
            return std::vector<Annotation*>::at(i);
    }
    return 0;
}

Annotation *Model::getAnnotationById(const std::string &id) const
{
    for (unsigned int i=0 ; i<std::vector<Annotation*>::size() ; i++)
    {
        if (std::vector<Annotation*>::at(i)->id() == id)
            return std::vector<Annotation*>::at(i);
    }
    return 0;
}

Image *Model::getImageAtPosition(double x, double y,double precision) const
{
    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
    {
        Image *image=std::vector<Image*>::at(i);
        if (x<image->x()-precision)
            continue;
        if (y<image->y()-precision)
            continue;
        if (x>image->x()+image->width()+precision)
            continue;
        if (y>image->y()+image->height()+precision)
            continue;
        return image;
    }
    return 0;
}

void Model::unselectAllVolumes()
{
    LOG_INFO("Unselect all volumes.");
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        std::vector<Volume*>::at(i)->setSelected(false);
}

void Model::unselectAllImages()
{
    LOG_INFO("Unselect all images.");
    for (unsigned int i=0 ; i<std::vector<Image*>::size() ; i++)
        std::vector<Image*>::at(i)->setSelected(false);
}

void Model::unselectAllDxf()
{
    LOG_INFO("Unselect all Dxf.");
    for (unsigned int i=0 ; i<std::vector<DxfContent*>::size() ; i++)
        std::vector<DxfContent*>::at(i)->setSelected(false);
}

void Model::unselectAllAnnotations()
{
    LOG_INFO("Unselect all annotations.");
    for (unsigned int i=0 ; i<std::vector<Annotation*>::size() ; i++)
        std::vector<Annotation*>::at(i)->setSelection(Annotation::NoSelection);
}

void Model::selectAllVolumes()
{
    LOG_INFO("Select all volumes.");
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        std::vector<Volume*>::at(i)->setSelected(true);
}

Mesh *Model::createMesh(bool &use_air_cavities, bool humidityMode, ErrorHandler *error) const
{
    Mesh *mesh=new Mesh;

    // Get all visible surfaces.
    std::vector<Line*> visible_surfaces=getAllVisibleNonIntersectedSurfaces();

    // Define if model used air cavities.
    use_air_cavities=false;

    // Get volumes cells.
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        std::vector<Cell*> cells=volume->createCells(humidityMode, error);

        if (volume->material() && volume->material()->id()=="106D6720-211C-4090-8C60-7CED671C8580")
            use_air_cavities=true;
        else if (volume->material() && volume->material()->id()=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
            use_air_cavities=true;

        // Abort by user.
        if (g_conducteo_aborted)
        {
            // Delete volume cells.
            for (unsigned int j=0 ; j<cells.size() ; j++)
                delete cells.at(j);
            cells.resize(0);
            for (unsigned int j=0 ; j<visible_surfaces.size() ; j++)
                delete visible_surfaces.at(j);
            visible_surfaces.resize(0);
            return mesh;
        }

        // Clear internal cells.
        volume->_internalCells.clear();

        for (unsigned int j=0 ; j<cells.size() ; j++)
        {
            Cell *cell=cells.at(j);

            // Abort by user.
            if (g_conducteo_aborted)
            {
                // Delete not processed cells.
                for (unsigned int k=j ; k<cells.size() ; k++)
                    delete cells.at(k);
                for (unsigned int j=0 ; j<visible_surfaces.size() ; j++)
                            delete visible_surfaces.at(j);
                        visible_surfaces.resize(0);
                return mesh;
            }

            // If cell center points to another volume
            // and cell doesn't intersect with volume
            // cell must be deleted.
            Point m=cell->squareMeshMiddle();
            Volume *volume_under=getVolumeAtPosition(m.x(), m.y());
            if (volume_under && volume_under!=volume)
            {
                if (!volume_under->intersectionWithMesh(cell))
                {
                    // If intersection between cell & initial volume
                    // and initial volume is higher as volume_under
                    // in stack: do nothing.
                    bool initial_volume_intersection=volume->intersectionWithMesh(cell);
                    bool delete_cell=!initial_volume_intersection;
                    if (initial_volume_intersection)
                    {
                        int idx_init=volumeStackPosition(volume);
                        int idx_under=volumeStackPosition(volume_under);
                        if (idx_init>=idx_under)
                            delete_cell=false;
                        else
                            delete_cell=true;
                    }

                    if (delete_cell)
                    {
                        // If volume under is purely internal volume: do nothing.
                        if (!volume_under->isIntoCell(cell))
                        {
                            delete cell;
                            continue;
                        }
                    }
                }
            }

            // Cell has no intersection with all visible surfaces.
            if (!intersection(cell, visible_surfaces))
            {
                mesh->push_back(cell);
                volume->_internalCells.push_back(cell);
                continue;
            }

            // Boundary cells.
            // Get all intersection lines between cell & visible surfaces
            // But only if they are in volume or cell.
            std::vector<Line*> lines=splitCell(cell, visible_surfaces, volume);
            std::vector<Cell*> new_cells=createCellsFromLines(cell, lines);

            delete cell;

            // Check if cell has some volume.
            for (unsigned int k=0 ; k<new_cells.size() ; k++)
            {
                if (!new_cells.at(k)->hasSurface())
                    delete new_cells.at(k);
                else
                {
                    mesh->push_back(new_cells.at(k));
                    volume->_internalCells.push_back(new_cells.at(k));
                }
            }
        }

        // All cells have been created here. We must now adjust cells to
        // internal volumes if needed.
        if (!volume->_internalVolumes.empty())
        {
            // Get all cells that have internal volumes.
            for (unsigned int j=0 ; j<volume->_internalCells.size() ; j++)
            {
                Cell *cell=volume->_internalCells.at(j);

                std::vector<Volume*> volumes;
                for (unsigned int k=0 ; k<volume->_internalVolumes.size() ; k++)
                {
                    if (volume->_internalVolumes.at(k)->isIntoCell(cell))
                        volumes.push_back(volume->_internalVolumes.at(k));
                }

                if (volumes.empty())
                    continue;

                // Get min volumes size.
                double min_size=0.0;
                for (unsigned int k=0 ; k<volumes.size() ; k++)
                {
                    Volume *volume=volumes.at(k);
                    double dx=volume->xMax()-volume->xMin();
                    double dy=volume->yMax()-volume->yMin();
                    double delta=std::min(dx, dy);

                    if (k==0 || delta<min_size)
                        min_size=delta;
                }

                // Refine cell. Create temporary volume.
                Volume v;
                v.setCellSize(min_size/2.0, 0);
                v.setMaterial(volume->material());
                
                for (unsigned int k=0 ; k<cell->size() ; k++)
                    v.push_back(new Line(*cell->at(k)));
                v.computeBounds();
                v.computeLines();

                std::vector<Line*> volume_lines;
                for (unsigned int k=0 ; k<volumes.size() ; k++)
                {
                    Volume *volume=volumes.at(k);
                    std::vector<Line*> v_lines=volume->getLines();
                    for (unsigned int j=0 ; j<v_lines.size() ; j++)
                    {
                        Line *l=v_lines.at(j);
                        volume_lines.push_back(l);
                    }
                }

                for (unsigned int j=0 ; j<v.size() ; j++)
                {
                    Line *l=(Line*)v.at(j);
                    volume_lines.push_back(l);
                }

                // Delete parent cell.
                double h_conductivity=cell->horizontalConductivity();
                double v_conductivity=cell->verticalConductivity();
                Material *material = cell->material();
                delete cell;
                for (unsigned int j=0 ; j<mesh->size() ; j++)
                    if (mesh->at(j)==cell)
                        mesh->at(j)=0;

                std::vector<Cell*> cells2=v.createCells(humidityMode, error);

                // Mesh reduction.
                // Refactoring needed.
                for (unsigned int j=0 ; j<cells2.size() ; j++)
                {
                    Cell *c=cells2.at(j);
                    c->setHorizontalConductivity(h_conductivity);
                    c->setVerticalConductivity(v_conductivity);
                    c->setMaterial(material);

                    // Abort by user.
                    if (g_conducteo_aborted)
                    {
                        // Delete not processed cells.
                        for (unsigned int k=j ; k<cells2.size() ; k++)
                            delete cells2.at(k);

                        for (unsigned int j=0 ; j<visible_surfaces.size() ; j++)
                            delete visible_surfaces.at(j);
                        visible_surfaces.resize(0);
                        return mesh;
                    }

                    // Boundary cells.
                    // Get all intersection lines between cell & visible surfaces
                    // But only if they are in volume or cell.
                    std::vector<Line*> lines=splitCell(c, volume_lines, volume);
                    std::vector<Cell*> new_cells=createCellsFromLines(c, lines, &v);

                    delete c;

                    // Check if cell has some volume.
                    for (unsigned int k=0 ; k<new_cells.size() ; k++)
                    {
                        if (!new_cells.at(k)->hasSurface() || !new_cells.at(k)->isClosed())
                            delete new_cells.at(k);
                        else
                        {
                            mesh->push_back(new_cells.at(k));
                        }
                    }
                }
            }
        }
    }

    for (unsigned int j=0 ; j<visible_surfaces.size() ; j++)
        delete visible_surfaces.at(j);
    visible_surfaces.resize(0);

    // Remove null cells.
    std::vector<Cell*> meshCells=*mesh;
    mesh->clear();

    for (unsigned int i=0 ; i<meshCells.size() ; i++)
        if (meshCells.at(i)!=0)
            mesh->push_back(meshCells.at(i));

    return mesh;
}

Mesh *Model::createHumidityMesh(ErrorHandler *error) const
{
    Mesh *mesh = 0;

    // Copy thermal mesh.
    if (_mesh)
    {
        LOG_INFO("Copying thermal mesh to humidity mesh.");
        mesh = new Mesh(*_mesh);

        // Set neighbors.
        std::map<Cell*, Cell*> data;

        for (unsigned int i=0 ; i<mesh->size() ; i++)
        {
            Cell *cell = mesh->at(i);
            Cell *old  = _mesh->at(i);

            data[old] = cell;
        }

        for (unsigned int i=0 ; i<mesh->size() ; i++)
        {
            Cell *cell = mesh->at(i);
            Cell *old  = _mesh->at(i);

            cell->useHumidityData();

            Cell *n1 = old->_neighbor1;
            Cell *n2 = old->_neighbor2;
            Cell *n3 = old->_neighbor3;
            Cell *n4 = old->_neighbor4;

            if (n1)
                cell->_neighbor1 = data[n1];
            if (n2)
                cell->_neighbor2 = data[n2];
            if (n3)
                cell->_neighbor3 = data[n3];
            if (n4)
                cell->_neighbor4 = data[n4];
        }
    }

    // Create from scratch.
    else
    {
        LOG_INFO("No thermal mesh available. Create humidity mesh from scratch.");

        bool dummy;
        mesh = createMesh(dummy, true, error);
    }

    return mesh;
}

std::vector<Cell*> Model::createCellsFromLines(Cell *ref_cell, const std::vector<Line*> &lines, Volume *ref_volume) const
{
    // Filter lines.
    std::vector<Line*> lines2;
    if (!ref_volume)
        lines2=lines;
    else
    {
        for (unsigned int k=0 ; k<lines.size() ; k++)
        {
            // Remove lines that are not separating current volume &
            // another volume.
            Line *line=lines.at(k);
            Point p1=line->nearMiddlePoint(true);
            Point p2=line->nearMiddlePoint(false);

            bool in_left=ref_volume->containsPoint(p1);
            bool in_right=ref_volume->containsPoint(p2);

            if (in_left || in_right)
                lines2.push_back(lines.at(k));
            else
                delete lines.at(k);
        }
    }

    std::vector<Line*> lines3=lines2;
    std::vector<Cell*> cells;

    bool lines_defined=!lines.empty();
    while (lines_defined)
    {
        // Get first not null line.
        int idx=-1;
        Line *l=firstLine(lines2, idx);
        if (!l)
            lines_defined=false;

        // Close cell.
        if (l)
        {
            std::vector<Line*> lines4=lines2;
            Cell *cell=new Cell(*ref_cell);
            cell->clean();
            cell->push_back(new Line(*l));
            lines2.at(idx)=0;
            lines4.at(idx)=0;

            Line *l2=nextLine(l, lines4, idx);
            while (l2)
            {
                cell->push_back(new Line(*l2));
                lines2.at(idx)=0;
                lines4.at(idx)=0;
                l2=nextLine(l2, lines4, idx);
            }
            
            // Remove duplicates.
            std::vector<Line *> cell_lines=*cell;
            cell->resize(0);

            for (unsigned int j=0 ; j<cell_lines.size() ; j++)
            {
                Line *line1=cell_lines.at(j);
                if (!line1)
                    continue;

                for (unsigned int i=0 ; i<cell_lines.size() ; i++)
                {
                    if (i!=j)
                    {
                        Line *line2=cell_lines.at(i);
                        if (!line2)
                            continue;

                        if (line1->containsPath(line2))
                        {
                            cell_lines.at(i)=0;
                            delete line2;
                        }
                    }
                }
            }

            for (unsigned int j=0 ; j<cell_lines.size() ; j++)
                if (cell_lines.at(j))
                    cell->push_back(cell_lines.at(j));
            
            cell->computeBounds();
            cell->closeCell();
            cells.push_back(cell);
        }
    }

    for (unsigned int i=0 ; i<lines3.size() ; i++)
        delete lines3.at(i);
    
    return cells;
}

Line *Model::nextLine(Line *line, const std::vector<Line*> &lines, int &idx) const
{
    idx=-1;
    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        Line *line2=lines.at(i);
        if (!line2)
            continue;
        if (line2==line)
            continue;

        Point p=line->end();
        Point t1=line2->begin();
        Point t2=line2->end();

        if (GeometryTools::equivalentPoints(p, t1) || GeometryTools::equivalentPoints(p, t2))
        {
            idx=i;
            return line2;
        }

        p=line->begin();
        if (GeometryTools::equivalentPoints(p, t1) || GeometryTools::equivalentPoints(p, t2))
        {
            idx=i;
            return line2;
        }
    }
    return 0;
}

Line *Model::firstLine(const std::vector<Line*> &lines, int &idx) const
{
    idx=-1;
    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        if (lines.at(i))
        {
            idx=i;
            return lines.at(i);
        }
    }
    return 0;
}

std::vector<Line*> Model::splitCell(Cell *cell, const std::vector<Line*> &surfaces, Volume *ref_volume) const
{
    // Copy surfaces.
    std::vector<Line*> lines;
    for (unsigned int j=0 ; j<surfaces.size() ; j++)
    {
        Line *line=surfaces.at(j);
        Line *line2=new Line(*line);
        lines.push_back(line2);
    }

    // Add cell lines.
    for (unsigned int j=0 ; j<cell->size() ; j++)
    {
        Line *line=cell->at(j);
        Line *line2=new Line(*line);
        lines.push_back(line2);
    }

    // Extract intersections.
    lines=extractNonIntersectedSurfaces(lines);

    // Remove lines that are not in cell & in volume.
    std::vector<Line*> s;
    for (unsigned int j=0 ; j<lines.size() ; j++)
    {
        Line *line=lines.at(j);
        Point p1=line->nearMiddlePoint(true);
        Point p2=line->nearMiddlePoint(false);

        if (!cell->squareMeshContainsPoint(p1) && !cell->squareMeshContainsPoint(p2))
        {
            delete line;
            continue;
        }

        Volume *volume1=getVolumeAtPosition(p1.x(), p1.y());
        Volume *volume2=getVolumeAtPosition(p2.x(), p2.y());

        if (volume1!=ref_volume && volume2!=ref_volume)
        {
            delete line;
            continue;
        }

        s.push_back(line);
    }

    // Remove duplicates.
    for (unsigned int j=0 ; j<s.size() ; j++)
    {
        Line *line1=s.at(j);
        if (!line1)
            continue;

        for (unsigned int i=0 ; i<s.size() ; i++)
        {
            if (i!=j)
            {
                Line *line2=s.at(i);
                if (!line2)
                    continue;

                if (line1->containsPath(line2))
                {
                    s.at(i)=0;
                    delete line2;
                }
            }
        }
    }

    std::vector<Line*> s3;
    for (unsigned int j=0 ; j<s.size() ; j++)
        if (s.at(j))
            s3.push_back(s.at(j));

    // Remove lines with not connected bounds.
    std::vector<Line*> s2;
    for (unsigned int j=0 ; j<s3.size() ; j++)
    {
        Line *line=s3.at(j);
        Point p1=line->begin();
        Point p2=line->end();

        int p1_c=0;
        int p2_c=0;

        for (unsigned int k=0 ; k<s3.size() ; k++)
        {
            if (k!=j)
            {
                Line *line2=s3.at(k);
                if (!line2)
                    continue;

                Point p3=line2->begin();
                Point p4=line2->end();

                if (GeometryTools::equivalentPoints(p1, p3))
                    p1_c++;
                else if (GeometryTools::equivalentPoints(p2, p3))
                    p2_c++;

                if (GeometryTools::equivalentPoints(p1, p4))
                    p1_c++;
                else if (GeometryTools::equivalentPoints(p2, p4))
                    p2_c++;
            }
        }

        if (p1_c>=1 && p2_c>=1)
            s2.push_back(line);
        else
        {
            delete line;
            s3.at(j)=0;
        }
    }

    return s2;
}

std::vector<Line*> Model::getAllVisibleNonIntersectedSurfaces() const
{
    std::vector<Line*> lines=getAllSurfaces();
    std::vector<Line*> surfaces=extractNonIntersectedSurfaces(lines);

    std::vector<Line*> s;
    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Line *line=surfaces.at(i);
        Point m=line->middlePoint();

        Point p1=line->nearMiddlePoint(true);
        Point p2=line->nearMiddlePoint(false);

        Volume *volume1=getVolumeAtPosition(p1.x(), p1.y());
        Volume *volume2=getVolumeAtPosition(p2.x(), p2.y());

        if (volume1==volume2)
        {
            delete line;
            continue;
        }
        s.push_back(line);
    }
    return s;
}

bool Model::intersection(Cell *cell, const std::vector<Line*> &lines) const
{
    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        Line *line1=lines.at(i);
        for (unsigned int j=0 ; j<cell->size() ; j++)
        {
            Line *line2=cell->at(j);
            if (GeometryTools::intersects(line1, line2))
                return true;
        }
    }
    return false;
}

inline double Model::round(double value) const
{
    return std::floor(value*1.0e+9+0.5)/1.0e+9;
}

#ifdef DISPLAY_MATRIX_CONTENT

std::string Model::matrixData(unsigned int row) const
{
    if (row<_matrixData.size())
        return _matrixData[row];
    return "Index out of bounds.";
}

std::vector<unsigned int> Model::matrixNeighbors(unsigned int row) const
{
    return _matrixNeighborCells[row];
}

#endif

void Model::generateMatrix(ErrorHandler *error)
{
    // Create mesh.
    if (_mesh)
        delete _mesh;

    // Define if model use air cavities.
    bool use_air_cavities=false;
    
    _mesh=createMesh(use_air_cavities, false, error);

    // Check for erreor.
    if (error && error->code()!=ErrorHandler::NoError)
    {
        delete _mesh;
        _mesh=0;
        return;
    }

    unsigned int N = _mesh->size();

    LOG_INFO("Thermal mesh size: " << N);

    DbMatrix matrix_coefficients;
    std::vector<CellLine> lineWithExternalBoundaryCondition;

    std::vector<double_triplet> coefficients;
    Eigen::VectorXd b(N);
    b.setZero();

    // Compute all cells centers & split into 2 lists.
    std::vector<Cell*> internal_cells;
    std::vector<Cell*> external_cells;
    std::vector<Cell*> external_lookup_table;
    for (unsigned int i=0 ; i<_mesh->size() ; i++)
    {
        Cell *cell=_mesh->at(i);

        if (cell->isInternal())
        {
            internal_cells.push_back(cell);
            if (!cell->_neighbor1->isInternal())
                external_lookup_table.push_back(cell);
            else if (!cell->_neighbor2->isInternal())
                external_lookup_table.push_back(cell);
            else if (!cell->_neighbor3->isInternal())
                external_lookup_table.push_back(cell);
            else if (!cell->_neighbor4->isInternal())
                external_lookup_table.push_back(cell);
        }
        else
        {
            cell->computeCenter();
            cell->removeUnusedPoints();
            external_cells.push_back(cell);
            external_lookup_table.push_back(cell);
        }
        
        cell->_matrixIdx=i;
    }

    // Create matrix & second member.
    createMatrix(matrix_coefficients, b, lineWithExternalBoundaryCondition, internal_cells, external_cells, external_lookup_table);

    // Store cells neighbors.
    for(DbMatrixIt it=matrix_coefficients.begin() ; it!=matrix_coefficients.end() ; it++)
    {
        unsigned int row_idx=it->first;
        DbRowMatrix &row=it->second;

        Cell *cell_i=_mesh->at(row_idx);
        cell_i->_neighbors.clear();

        for(DbRowMatrixIt it2=row.begin() ; it2!=row.end() ; it2++)
        {
            unsigned int col_idx=it2->first;
            if (row_idx==col_idx)
                continue;
            Cell *cell_j=_mesh->at(col_idx);
            cell_i->_neighbors.push_back(cell_j);
        }
    }

    // Compute air cavities & restart matrix creation.
    if (use_air_cavities)
    {
        // List air cells.
        std::vector<Cell*> _unventilated;
        std::vector<Cell*> _partiallyVentilated;

        for (unsigned int i=0 ; i<_mesh->size() ; i++)
        {
            if (_mesh->at(i)->_airCellType==1)
                _unventilated.push_back(_mesh->at(i));
            else if (_mesh->at(i)->_airCellType==2)
                _partiallyVentilated.push_back(_mesh->at(i));
        }

        // Create air cavities.
        std::vector<AirCavityFromMesh*> cavities=createCavities(_unventilated, _partiallyVentilated);

        // Update air cavities conductivities.
        for (unsigned int i=0 ; i<cavities.size() ; i++)
        {
            cavities.at(i)->finalize();
            delete cavities.at(i);
        }
        cavities.resize(0);

        // Recreate matrix & vector.
        matrix_coefficients.clear();
        b.setZero();
        lineWithExternalBoundaryCondition.clear();
        createMatrix(matrix_coefficients, b, lineWithExternalBoundaryCondition, internal_cells, external_cells, external_lookup_table);
    }

#ifdef DISPLAY_MATRIX_CONTENT
    _matrixData.clear();
    _matrixNeighborCells.clear();
#endif

    // Finalize matrix creation.
#ifdef DISPLAY_MATRIX_CONTENT
    _matrixNeighborCells.resize(N);
    _matrixData.resize(N);
#endif
    for(DbMatrixIt it=matrix_coefficients.begin() ; it!=matrix_coefficients.end() ; it++)
    {
        unsigned int row_idx=it->first;
        DbRowMatrix &row=it->second;

#ifdef DISPLAY_MATRIX_CONTENT
        std::string matrix_data="[Cell "+ToolBox::intToString(row_idx)+"] ";
        std::vector<unsigned int> neighbors;
#endif

        for(DbRowMatrixIt it2=row.begin() ; it2!=row.end() ; it2++)
        {
            unsigned int col_idx=it2->first;
            double value=it2->second;

#ifdef DISPLAY_MATRIX_CONTENT
            neighbors.push_back(col_idx);
            if (row_idx!=col_idx)
                matrix_data+="\n   > [Cell "+ToolBox::intToString(col_idx)+"] "+ToolBox::doubleToString(value);
            else
                matrix_data+="\n   * [Cell "+ToolBox::intToString(col_idx)+"] "+ToolBox::doubleToString(value);
#endif

            coefficients.push_back(double_triplet(row_idx, col_idx, value));
        }
#ifdef DISPLAY_MATRIX_CONTENT
        matrix_data+="\n   = [second] "+ToolBox::doubleToString(b[row_idx]);
        _matrixData.at(row_idx)=matrix_data;
        _matrixNeighborCells.at(row_idx)=neighbors;
#endif
    }

    // Create sparse matrix.
    Eigen::SparseMatrix<double> A(N,N);
    A.setFromTriplets(coefficients.begin(), coefficients.end());

#ifdef CHECK_MATRIX
    // Matrix must be symetric.
    for (unsigned int i=0 ; i<N ; i++)
    {
        bool ok=true;
        for (unsigned int j=0 ; j<N ; j++)
        {
            double v1=A.coeff(i, j);
            double v2=A.coeff(j, i);
            if (v1!=v2)
            {
                ok=false;
            }
        }
        if (!ok)
        {
        }
    }
#endif

    // Solve system.
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double> > cg(A);
    cg.setTolerance(1.0e-18);
    Eigen::VectorXd x=cg.solve(b);

    // Dispatch results.
    for (unsigned int i=0 ; i<_mesh->size() ; i++)
    {
        Cell *cell_i=_mesh->at(i);
        cell_i->setTemperature(x[i]);
    }

    // Finalize computation.

    // Reset surface.
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        surface->setFlux(0.0);
        surface->setMinimalTemperature(0.0);
        surface->setMaximalTemperature(0.0);
        surface->setMinimalRh(0.0);
        surface->setMaximalRh(0.0);
    }

    // Find surface data.
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface = std::vector<Surface*>::at(i);
        surface->setFlux(0.0);

        BoundaryCondition *boundary = surface->boundaryCondition();
        if (!boundary)
            continue;

        double Re = boundary->resistance();
        double Te = surface->temperature();

        bool min_temp_defined = false;
        bool max_temp_defined = false;
        bool min_rh_defined   = false;
        bool max_rh_defined   = false;

        for (unsigned int j=0 ; j<lineWithExternalBoundaryCondition.size() ; j++)
        {
            CellLine &data = lineWithExternalBoundaryCondition.at(j);
            if (data._surface != surface)
                continue;
            
            Cell *cell = _mesh->at(data._cellIdx);

            // Compute local flux.
            double flux = data._area*(Te - cell->temperature()) / (Re + data._internalResistance) * data._scalarProduct;
            surface->setFlux(surface->flux() + flux);

            // Get temperature.
            double tp = Te - flux / data._area * Re / data._scalarProduct;

            if (tp < surface->minimalTemperature() || !min_temp_defined)
            {
                surface->setMinimalTemperature(tp);
                surface->setMinimalTemperatureLocation(data._superficialPoint);
                min_temp_defined = true;
            }
            if (tp > surface->maximalTemperature() || !max_temp_defined)
            {
                surface->setMaximalTemperature(tp);
                surface->setMaximalTemperatureLocation(data._superficialPoint);
                max_temp_defined = true;
            }

            // Condensation.
            if (_computationType == GlaserCondensation)
            {
                double psatTe = getSaturationPression(Te);
                double psatTp = getSaturationPression(tp);

                double rh = boundary->rh() * psatTe / psatTp;
                if (rh < surface->minimalRh() || !min_rh_defined)
                {
                    surface->setMinimalRh(rh);
                    min_rh_defined = true;
                }
                if (rh > surface->maximalRh() || !max_rh_defined)
                {
                    surface->setMaximalRh(rh);
                    max_rh_defined = true;
                }
            }
        }
    }
}

void Model::generateHumidityMatrix(ErrorHandler *error)
{
    // Create mesh.
    if (_glaserMesh)
        delete _glaserMesh;

    _glaserMesh = createHumidityMesh(error);

    // Check for error.
    if (error && error->code()!=ErrorHandler::NoError)
    {
        delete _glaserMesh;
        _glaserMesh = 0;
        return;
    }

    unsigned int N = _mesh->size();

    LOG_INFO("Humidity mesh size: " << N);

    DbMatrix matrix_coefficients;
    std::vector<CellLine> lineWithExternalBoundaryCondition;

    std::vector<double_triplet> coefficients;
    Eigen::VectorXd b(N);
    b.setZero();

    // Compute all cells centers & split into 2 lists.
    std::vector<Cell*> internal_cells;
    std::vector<Cell*> external_cells;
    std::vector<Cell*> external_lookup_table;
    for (unsigned int i=0 ; i<_glaserMesh->size() ; i++)
    {
        Cell *cell=_glaserMesh->at(i);

        if (cell->isInternal())
        {
            internal_cells.push_back(cell);
            if (!cell->_neighbor1->isInternal())
                external_lookup_table.push_back(cell);
            else if (!cell->_neighbor2->isInternal())
                external_lookup_table.push_back(cell);
            else if (!cell->_neighbor3->isInternal())
                external_lookup_table.push_back(cell);
            else if (!cell->_neighbor4->isInternal())
                external_lookup_table.push_back(cell);
        }
        else
        {
            cell->computeCenter();
            cell->removeUnusedPoints();
            external_cells.push_back(cell);
            external_lookup_table.push_back(cell);
        }

        cell->_matrixIdx=i;
    }

    // Create matrix & second member.
    createHumidityMatrix(matrix_coefficients, b, lineWithExternalBoundaryCondition, internal_cells, external_cells, external_lookup_table);

    // Store cells neighbors.
    for(DbMatrixIt it=matrix_coefficients.begin() ; it!=matrix_coefficients.end() ; it++)
    {
        unsigned int row_idx=it->first;
        DbRowMatrix &row=it->second;

        Cell *cell_i=_glaserMesh->at(row_idx);
        cell_i->_neighbors.clear();

        for(DbRowMatrixIt it2=row.begin() ; it2!=row.end() ; it2++)
        {
            unsigned int col_idx=it2->first;
            if (row_idx==col_idx)
                continue;
            Cell *cell_j=_glaserMesh->at(col_idx);
            cell_i->_neighbors.push_back(cell_j);
        }
    }

    for(DbMatrixIt it=matrix_coefficients.begin() ; it!=matrix_coefficients.end() ; it++)
    {
        unsigned int row_idx=it->first;
        DbRowMatrix &row=it->second;

        for(DbRowMatrixIt it2=row.begin() ; it2!=row.end() ; it2++)
        {
            unsigned int col_idx=it2->first;
            double value=it2->second;

            coefficients.push_back(double_triplet(row_idx, col_idx, value));
        }
    }

    // Create sparse matrix.
    Eigen::SparseMatrix<double> A(N,N);
    A.setFromTriplets(coefficients.begin(), coefficients.end());

    // Solve system.
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double> > cg(A);
    cg.setTolerance(1.0e-18);
    Eigen::VectorXd x=cg.solve(b);

    // Dispatch results.
    for (unsigned int i=0 ; i<_glaserMesh->size() ; i++)
    {
        Cell *cell_i=_glaserMesh->at(i);
        cell_i->setTemperature(x[i]);
    }
}

std::vector<AirCavityFromMesh*> Model::createCavities(std::vector<Cell*> &unventilated, std::vector<Cell*> &partiallyVentilated) const
{
    std::vector<AirCavityFromMesh*> cavities;

    for (unsigned int i=0 ; i<unventilated.size() ; i++)
    {
        Cell *cell=unventilated.at(i);
        if (!cell)
            continue;

        AirCavityFromMesh *cavity=new AirCavityFromMesh;
        cavity->setType(RegularAirCavity::Unventilated);
        cavities.push_back(cavity);

        cavity->addCell(cell);
        unventilated.at(i)=0;

        while (addCellsIntoCavity(cavity, unventilated))
        {
        }
    }

    for (unsigned int i=0 ; i<partiallyVentilated.size() ; i++)
    {
        Cell *cell=partiallyVentilated.at(i);
        if (!cell)
            continue;

        AirCavityFromMesh *cavity=new AirCavityFromMesh;
        cavity->setType(RegularAirCavity::PartiallyVentilated);
        cavities.push_back(cavity);

        cavity->addCell(cell);
        partiallyVentilated.at(i)=0;

        while (addCellsIntoCavity(cavity, partiallyVentilated))
        {
        }
    }

    return cavities;
}

unsigned int Model::addCellsIntoCavity(AirCavityFromMesh *cavity, std::vector<Cell*> &cells) const
{
    unsigned int n=0;
    
    for (unsigned int i=0 ; i<cavity->size() ; i++)
    {
        Cell *cell_i=cavity->at(i);

        // Loop on all neighbors.
        for (unsigned j=0 ; j<cell_i->_neighbors.size() ; j++)
        {
            Cell *cell_j=cell_i->_neighbors.at(j);
            if (!cell_j)
                continue;

            // Check if cell_j is into cells vector.
            int idx=-1;
            for (unsigned int k=0 ; k<cells.size() ; k++)
            {
                if (cells.at(k)==cell_j)
                {
                    idx=k;
                    break;
                }
            }

            if (idx==-1)
            {
                cell_i->_neighbors.at(j)=0;
                continue;
            }

            // Add cell_j into cavity.
            cavity->push_back(cell_j);
            cells.at(idx)=0;
            n++;
        }
    }
    return n;
}

void Model::createMatrix(DbMatrix &matrix_coefficients, Eigen::VectorXd &b, std::vector<CellLine> &lineWithExternalBoundaryCondition, const std::vector<Cell*> &internal_cells, const std::vector<Cell*> &external_cells, const std::vector<Cell*> &external_lookup_table) const
{
    // Create matrix & second member.
    // Internal cells.
    for (unsigned int i=0 ; i<internal_cells.size() ; i++)
    {
        Cell *cell_i=internal_cells.at(i);
        double conductivity_x=round(cell_i->horizontalConductivity());
        double conductivity_y=round(cell_i->verticalConductivity());
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_matrixIdx]=-2.0*(conductivity_x+conductivity_y);
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor1->_matrixIdx]=conductivity_x;
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor2->_matrixIdx]=conductivity_y;
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor3->_matrixIdx]=conductivity_x;
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor4->_matrixIdx]=conductivity_y;
    }

    // External cells.
    for (unsigned int i=0 ; i<external_cells.size() ; i++)
    {
        Cell *cell_i=external_cells.at(i);

        // Conduction between cells.
        for (unsigned int j=0 ; j<external_lookup_table.size() ; j++)
        {
            Cell *cell_j=external_lookup_table.at(j);

            if (cell_i!=cell_j)
            {

                // Optimization.
                if (cell_i->xMax()+1.0e-4<cell_j->xMin())
                    continue;
                if (cell_i->yMax()+1.0e-4<cell_j->yMin())
                    continue;
                if (cell_j->xMax()+1.0e-4<cell_i->xMin())
                    continue;
                if (cell_j->yMax()+1.0e-4<cell_i->yMin())
                    continue;

                for (unsigned int k=0 ; k<cell_i->size() ; k++)
                {
                    Line *cell_border=cell_i->at(k);

                    // Abort by user.
                    if (g_conducteo_aborted)
                        return;
                    
                    // Get common surface.
                    double S;
                    S=cell_j->commonArea(cell_border);
                    if (S<1.0e-9)
                        continue;

                    // Get Ti & Tj points.
                    Point Ti=cell_i->center();
                    Point Tj=cell_j->center();
                    Line TiTj;
                    TiTj.setBegin(Ti, 0);
                    TiTj.setEnd(Tj, 0);

                    // Get Ti/Tj & cell border intersection point.
                    std::vector<Point> ip;
                    ip=GeometryTools::intersectionPoint(cell_border, &TiTj);
                    if (ip.size()!=1)
                        continue;

                    // Point must be on boundary for other cell.
                    bool on_boundary;
                    on_boundary=cell_j->pointOnBoundary(ip.at(0));
                    if (!on_boundary)
                        continue;

                    double dx1=ip.at(0).x()-Ti.x();
                    double dy1=ip.at(0).y()-Ti.y();
                    double d1=sqrt(dx1*dx1+dy1*dy1);

                    double dx2=ip.at(0).x()-Tj.x();
                    double dy2=ip.at(0).y()-Tj.y();
                    double d2=sqrt(dx2*dx2+dy2*dy2);

                    // Get orthogonal scalar product.
                    double sc_pdt=std::fabs(GeometryTools::scalarOrthogonalProduct(cell_border, &TiTj));

                    // Compute coefficient.
                    double R=d1/cell_i->conductivity(&TiTj)+d2/cell_j->conductivity(&TiTj);
                    double coefficient=round(S/R*sc_pdt);

                    matrix_coefficients[cell_i->_matrixIdx][cell_i->_matrixIdx]+=-coefficient;
                    matrix_coefficients[cell_i->_matrixIdx][cell_j->_matrixIdx]+=coefficient;
                }
            }
        }

        for (unsigned int k=0 ; k<cell_i->size() ; k++)
        {
            Line *cell_border=cell_i->at(k);

            // Convection from boundary conditions.
            for (unsigned int s=0 ; s<std::vector<Surface*>::size() ; s++)
            {
                // Abort by user.
                if (g_conducteo_aborted)
                    return;

                Surface *surface=std::vector<Surface*>::at(s);
                if (!surface->boundaryCondition())
                    continue;

                double Rse=surface->boundaryCondition()->resistance();
                double Tse=surface->temperature();

                // No adiabatic.
                if (Rse<=0.0)
                    continue;

                // Get common surface.
                double S;
                S=surface->commonArea(cell_border);
                if (S<1.0e-9)
                    continue;

                Point Ts=cell_border->middlePoint();
                Point Ti=cell_i->center();

                Line TiTj;
                TiTj.setBegin(Ti, 0);
                TiTj.setEnd(Ts, 0);

                double dx=Ts.x()-Ti.x();
                double dy=Ts.y()-Ti.y();
                double d=sqrt(dx*dx+dy*dy);

                // Get orthogonal scalar product.
                double sc_pdt=std::fabs(GeometryTools::scalarOrthogonalProduct(cell_border, &TiTj));

                // Compute coefficient.
                double R=d/cell_i->conductivity(&TiTj)+Rse;
                double coefficient=round(S/R*sc_pdt);

                matrix_coefficients[cell_i->_matrixIdx][cell_i->_matrixIdx]+=-coefficient;
                b[cell_i->_matrixIdx]+=-Tse*coefficient;

                CellLine data;
                data._cellIdx=cell_i->_matrixIdx;
                data._lineIdx=k;
                data._area=S;
                data._internalResistance=d/cell_i->conductivity(&TiTj);
                data._surface=surface;
                data._superficialPoint=Ts;
                data._scalarProduct=sc_pdt;
                lineWithExternalBoundaryCondition.push_back(data);
            }
        }
    }
}

void Model::createHumidityMatrix(DbMatrix &matrix_coefficients, Eigen::VectorXd &b, std::vector<CellLine> &lineWithExternalBoundaryCondition, const std::vector<Cell *> &internal_cells, const std::vector<Cell *> &external_cells, const std::vector<Cell *> &external_lookup_table) const
{
    // Create matrix & second member.
    // Internal cells.
    for (unsigned int i=0 ; i<internal_cells.size() ; i++)
    {
        Cell *cell_i=internal_cells.at(i);
        double conductivity_x=round(cell_i->horizontalConductivity());
        double conductivity_y=round(cell_i->verticalConductivity());
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_matrixIdx]=-2.0*(conductivity_x+conductivity_y);
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor1->_matrixIdx]=conductivity_x;
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor2->_matrixIdx]=conductivity_y;
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor3->_matrixIdx]=conductivity_x;
        matrix_coefficients[cell_i->_matrixIdx][cell_i->_neighbor4->_matrixIdx]=conductivity_y;
    }

    for (unsigned int i=0 ; i<external_cells.size() ; i++)
    {
        Cell *cell_i=external_cells.at(i);

        // Conduction between cells.
        for (unsigned int j=0 ; j<external_lookup_table.size() ; j++)
        {
            Cell *cell_j=external_lookup_table.at(j);

            if (cell_i!=cell_j)
            {

                // Optimization.
                if (cell_i->xMax()+1.0e-4<cell_j->xMin())
                    continue;
                if (cell_i->yMax()+1.0e-4<cell_j->yMin())
                    continue;
                if (cell_j->xMax()+1.0e-4<cell_i->xMin())
                    continue;
                if (cell_j->yMax()+1.0e-4<cell_i->yMin())
                    continue;

                for (unsigned int k=0 ; k<cell_i->size() ; k++)
                {
                    Line *cell_border=cell_i->at(k);

                    // Abort by user.
                    if (g_conducteo_aborted)
                        return;

                    // Get common surface.
                    double S;
                    S=cell_j->commonArea(cell_border);
                    if (S<1.0e-9)
                        continue;

                    // Get Ti & Tj points.
                    Point Ti=cell_i->center();
                    Point Tj=cell_j->center();
                    Line TiTj;
                    TiTj.setBegin(Ti, 0);
                    TiTj.setEnd(Tj, 0);

                    // Get Ti/Tj & cell border intersection point.
                    std::vector<Point> ip;
                    ip=GeometryTools::intersectionPoint(cell_border, &TiTj);
                    if (ip.size()!=1)
                        continue;

                    // Point must be on boundary for other cell.
                    bool on_boundary;
                    on_boundary=cell_j->pointOnBoundary(ip.at(0));
                    if (!on_boundary)
                        continue;

                    double dx1=ip.at(0).x()-Ti.x();
                    double dy1=ip.at(0).y()-Ti.y();
                    double d1=sqrt(dx1*dx1+dy1*dy1);

                    double dx2=ip.at(0).x()-Tj.x();
                    double dy2=ip.at(0).y()-Tj.y();
                    double d2=sqrt(dx2*dx2+dy2*dy2);

                    // Get orthogonal scalar product.
                    double sc_pdt=std::fabs(GeometryTools::scalarOrthogonalProduct(cell_border, &TiTj));

                    // Compute coefficient.
                    double R=d1/cell_i->conductivity(&TiTj)+d2/cell_j->conductivity(&TiTj);
                    double coefficient=round(S/R*sc_pdt);

                    matrix_coefficients[cell_i->_matrixIdx][cell_i->_matrixIdx]+=-coefficient;
                    matrix_coefficients[cell_i->_matrixIdx][cell_j->_matrixIdx]+=coefficient;
                }
            }
        }

        for (unsigned int k=0 ; k<cell_i->size() ; k++)
        {
            Line *cell_border=cell_i->at(k);

            // Convection from boundary conditions.
            for (unsigned int s=0 ; s<std::vector<Surface*>::size() ; s++)
            {
                // Abort by user.
                if (g_conducteo_aborted)
                    return;

                Surface *surface=std::vector<Surface*>::at(s);
                if (!surface->boundaryCondition())
                    continue;

                double Rse = surface->boundaryCondition()->resistance();
                double Tse = surface->temperature();
                double Rhe = surface->boundaryCondition()->rh();

                // No adiabatic.
                if (Rse<=0.0)
                    continue;

                // Get common surface.
                double S;
                S=surface->commonArea(cell_border);
                if (S<1.0e-9)
                    continue;

                Point Ts = cell_border->middlePoint();
                Point Ti = cell_i->center();

                Line TiTj;
                TiTj.setBegin(Ti, 0);
                TiTj.setEnd(Ts, 0);

                double dx = Ts.x()-Ti.x();
                double dy = Ts.y()-Ti.y();
                double d  = sqrt(dx * dx + dy * dy);

                // Get orthogonal scalar product.
                double sc_pdt = std::fabs(GeometryTools::scalarOrthogonalProduct(cell_border, &TiTj));

                // Compute coefficient.
                double R = d / cell_i->conductivity(&TiTj) /* + Rse = 0 */;
                double coefficient = round(S / R * sc_pdt);

                // Compute partial pressure for water.
                double pvs = getSaturationPression(Tse);
                double pp = pvs * Rhe;

                matrix_coefficients[cell_i->_matrixIdx][cell_i->_matrixIdx] += - coefficient;
                b[cell_i->_matrixIdx] += - pp * coefficient;    //< Tse is replaced by pp.

                CellLine data;
                data._cellIdx=cell_i->_matrixIdx;
                data._lineIdx=k;
                data._area=S;
                data._internalResistance=d/cell_i->conductivity(&TiTj);
                data._surface=surface;
                data._superficialPoint=Ts;
                data._scalarProduct=sc_pdt;
                lineWithExternalBoundaryCondition.push_back(data);
            }
        }
    }
}

Mesh *Model::mesh() const
{
    if (_running)
        return 0;
    return _mesh;
}

Mesh *Model::glaserMesh() const
{
    return _glaserMesh;
}

double Model::globalPositiveFlux() const
{
    double flux=0.0;
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        flux+=std::fabs(surface->flux());
    }
    return flux;
}

double Model::globalFlux() const
{
    double flux=0.0;
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        flux+=surface->flux();
    }
    return flux;
}

void Model::abort()
{
    g_conducteo_aborted=true;
    if (_currentError)
        _currentError->setCode(ErrorHandler::AbortByUser);
}

bool Model::isAborted() const
{
    return g_conducteo_aborted;
}

void Model::setCurrentErrorHandler(ErrorHandler *error)
{
    _currentError=error;
}

void Model::compute(ErrorHandler *error, bool compute_environments)
{
    LOG_INFO("Compute model.");

    g_conducteo_aborted=false;
    _psiCoefficient=0.0;
    _psiCoefficientComputed=false;

    // Create volume lines list.
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        std::vector<Volume*>::at(i)->computeLines();
        std::vector<Volume*>::at(i)->computeBounds();
    }

    // Store internal volumes.
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        std::vector<Volume*>::at(i)->setModelForInternalVolumes(this);

    // Compute 1D flux.
    // (for ThermalBridge & ThermalTransmission).
    if (_computationType == ThermalBridge || _computationType == ThermalTransmission)
    {
        if (_callback)
            _callback(WithoutTBFlux, 0, 0, _caller);
        _flux1d=compute1dFlux();
    }
    else
        _flux1d = 0.0;

    CallbackType cb_type=CouplingCoefficients;
    if (compute_environments)
        cb_type=PhysicalFlux;

    // Compute coupling coefficients.
    if (compute_environments && _computationType == ThermalBridge)
    {
        if (_callback)
            _callback(CouplingCoefficients, 0, 0, _caller);
        CouplingCoefficientComputation coupling(this, _callback, _caller);
        _environmentResults=coupling.compute();
    }
    else
        _environmentResults.clear();

    // Initialize volume refinment.
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
        std::vector<Volume*>::at(i)->initRefinment();

    if (g_conducteo_aborted)
    {
        setError(error, AbortByUser);
        return;
    }

    // Compute level.
    if (_callback)
        _callback(cb_type, 1, 0, _caller);
    generateMatrix(error);
    if (error && error->code()!=ErrorHandler::NoError)
        return;

    if (g_conducteo_aborted)
    {
        setError(error, AbortByUser);
        return;
    }

    // Compute global flux.
    _positiveFlux=globalPositiveFlux();

    double percent=10.0;
    int n=1;

    // Run computation with refined meshes.
    while (percent>1.0)
    {
        n++;
        _positiveFluxOld=_positiveFlux;

        // Increase volume refinment.
        for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
            std::vector<Volume*>::at(i)->increaseRefinment();

        if (g_conducteo_aborted)
        {
            setError(error, AbortByUser);
            return;
        }

        // Compute level.
        if (_callback)
            _callback(cb_type, n, 0, _caller);
        generateMatrix(error);
        if (error && error->code()!=ErrorHandler::NoError)
            return;

        if (g_conducteo_aborted)
        {
            setError(error, AbortByUser);
            return;
        }

        // Get global flux.
        _positiveFlux=globalPositiveFlux();

        // If current positive flux is null: all fluxes are null.
        // So we suppose we have converged.
        if (_positiveFlux < 1.0e-8 && _positiveFluxOld < 1.0e-8)
            percent = 0.0;
        else
            percent=std::fabs(_positiveFluxOld-_positiveFlux)/_positiveFluxOld*100.0;
    }

    // Compute residual flux.
    _residualFlux=globalFlux();

    // Compute 2D flux.
    _flux2d=compute2dFlux();

    // Compute psi coefficient.
    _psiCoefficient=computePsiCoefficient(&_psiCoefficientComputed);

    // Finalize mesh.
    for (unsigned int i=0 ; i<_mesh->size() ; i++)
        _mesh->at(i)->closeCell();

    // Compute Glaser humidity.
    // Disabled for Glaser model (avoid infinite loop).
    if (_computationType == GlaserCondensation)
    {
        if (_callback)
            _callback(GlaserComputation, 0, 0, _caller);

        LOG_INFO("Creating Glaser model.");
        computeHumidity(error);
    }

    if (g_conducteo_aborted)
    {
        setError(error, AbortByUser);
        return;
    }
    _validResults=true;
}

void Model::computeHumidity(ErrorHandler *error)
{
    LOG_INFO("Compute humidity model.");

    _condensation = false;

    generateHumidityMatrix(error);

    if (error && error->code()!=ErrorHandler::NoError)
        return;

    LOG_INFO("Finalize humidity mesh cells.");

    // Finalize mesh.
    for (unsigned int i=0 ; i<_glaserMesh->size() ; i++)
        _glaserMesh->at(i)->closeCell();

    LOG_INFO("Report humidity results.");

    // Report RH.
    for (unsigned int i=0 ; i<_glaserMesh->size() ; i++)
    {
        Cell *cell  = _glaserMesh->at(i);
        Cell *tCell = _mesh->at(i);

        double temperature = tCell->temperature();
        double pvs         = getSaturationPression(temperature);

        if (cell->temperature() > pvs)
            cell->setRh(1.0);
        else
            cell->setRh(cell->temperature() / pvs);

        if (!_condensation && cell->rh() >= 1.0)
            _condensation = true;
    }

    LOG_INFO("Humidity computation done.");
}

void Model::deleteObject(bool d)
{
    _deleteObject=d;
}

bool Model::deleteObject() const
{
    return _deleteObject;
}

void Model::setRunning(bool running)
{
    _running=running;
}

bool Model::running() const
{
    return _running;
}

unsigned int Model::numberOfNodes() const
{
    if (!_mesh)
        return 0;
    return _mesh->size();
}

double Model::fluxRelativeVariation() const
{
    if (_positiveFluxOld < 1.0e-8)
        return 0.0;
    return std::fabs(_positiveFluxOld-_positiveFlux)/_positiveFluxOld*100.0;
}

double Model::fluxSum() const
{
    return _residualFlux/_positiveFlux;
}

double Model::flux2d() const
{
    return _flux2d;
}

double Model::transmissionCoefficient() const
{
    if (!_psiCoefficientComputed)
        return 0.0;

    double l=transmissionCoefficientLength();
    if (l==0.0)
        return 0.0;

    return _psiCoefficient/l;
}

double Model::equivalentThermalConductivity() const
{
    if (!_psiCoefficientComputed)
        return 0.0;

    if (_computationType != EquivalentThermalConductivity)
        return 0.0;

    if (_equivalentConductivityWidth == 0.0)
        return 0.0;

    double dT = (_flux2d - _flux1d) / _psiCoefficient;

    return _flux2d * _equivalentConductivityThickness / (_equivalentConductivityWidth * dT - _equivalentConductivityResistanceSum * _flux2d);
}

double Model::flux1d() const
{
    double en13370Flux=0.0;
    if (_en13370)
        en13370Flux=_en13370->flux();
    return _flux1d+en13370Flux;
}

double Model::psiCoefficient() const
{
    return _psiCoefficient;
}

bool Model::psiCoefficientComputed() const
{
    return _psiCoefficientComputed;
}

void model1dCallback(Model::CallbackType type, int level, int total, void *pthis)
{
    Model *model=(Model*) pthis;
    model->set1dLevel(level);
}

void Model::set1dLevel(int level)
{
    if (_callback)
        _callback(WithoutTBFlux, level, 0, _caller);
}

double Model::compute1dFlux() const
{
    if (!_1dModel)
        return 0.0;

    _1dModel->setCallback(model1dCallback, (void*) this);
    _1dModel->compute(0, false);
    _1dModel->setCallback(0, 0);
    return _1dModel->flux2d();
}

double Model::compute2dFlux() const
{
    double flux=0.0;
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        if (surface->flux()>0.0)
            flux+=surface->flux();
    }
    return flux;
}

double Model::computePsiCoefficient(bool *success) const
{
    // List all boundary conditions temperatures.
    std::vector<double> temperatures;
    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        BoundaryCondition *bc=surface->boundaryCondition();
        if (!bc)
            continue;

        if (bc->id().empty())
            continue;

        temperatures.push_back(bc->temperature());
    }

    // Make unique list of boundary temperatures.
    std::sort(temperatures.begin(), temperatures.end());
    std::vector<double>::iterator it=std::unique(temperatures.begin(), temperatures.end());
    temperatures.resize(std::distance(temperatures.begin(),it));

    // If number of temperatures is different of 2, psi coefficient
    // cannot be computed.
    if (temperatures.size()!=2)
    {
        if (success)
            *success=false;
        return 0.0;
    }

    double dt=std::fabs(temperatures.at(1)-temperatures.at(0));
    if (success)
        *success=true;

    return (flux2d()-flux1d())/dt;
}

std::vector<EnvironmentResult> Model::environmentResults() const
{
    return _environmentResults;
}

double Model::temperatureAtPosition(double x, double y, bool *success) const
{
    *success=false;

    Cell *c = cellAtPosition(x, y);
    if (!c)
        return 0.0;

    *success = true;
    return c->temperature();
}

double Model::humidityAtPosition(double x, double y, bool *success) const
{
    *success = false;

    if (!_glaserMesh)
        return 0.0;

    Cell *c = cellAtPosition(_glaserMesh, x, y);
    if (!c)
        return 0.0;

    *success = true;

    return c->rh();
}

double Model::getSaturationPression(double temperature) const
{
    if (temperature >= 0)
        return 610.5 * exp(17.269 * temperature / (temperature + 237.3));

    return 610.5 * exp(21.875 * temperature / (temperature + 265.5));
}

double Model::pressureAtPosition(double x, double y, bool *success) const
{
    *success = false;

    if (!_glaserMesh)
        return 0.0;

    Cell *c = cellAtPosition(_glaserMesh, x, y);
    if (!c)
        return 0.0;

    *success = true;

    // In Glaser mode pressure is named 'temperature'.
    return c->temperature();
}

Cell *Model::cellAtPosition(double x, double y) const
{
    return cellAtPosition(_mesh, x, y);
}

Cell *Model::cellAtPosition(Mesh *mesh, double x, double y) const
{
    if (!mesh)
        return 0;

    Point p;
    p.setX(x);
    p.setY(y);

    // Find cell containing position.
    for (unsigned int i=0 ; i<mesh->size() ; i++)
    {
        Cell *c = mesh->at(i);
        if (!c->containsPoint(p))
            continue;
        return c;
    }

    return 0;
}

void Model::setCallback(ModelComputationCallback cb, void *p)
{
    _callback=cb;
    _caller=p;
}

void Model::bringVolumeUp(Volume *volume)
{
    int pos=volumeStackPosition(volume);
    if (pos>=int(std::vector<Volume*>::size()-1))
        return;

    Volume *v=std::vector<Volume*>::at(pos+1);
    std::vector<Volume*>::at(pos)=v;
    std::vector<Volume*>::at(pos+1)=volume;
}

void Model::bringVolumeDown(Volume *volume)
{
    int pos=volumeStackPosition(volume);
    if (pos<=0)
        return;

    Volume *v=std::vector<Volume*>::at(pos-1);
    std::vector<Volume*>::at(pos)=v;
    std::vector<Volume*>::at(pos-1)=volume;
}

void Model::bringImageUp(Image *image)
{
    int pos=imageStackPosition(image);
    if (pos>=int(std::vector<Image*>::size()-1))
        return;

    Image *i=std::vector<Image*>::at(pos+1);
    std::vector<Image*>::at(pos)=i;
    std::vector<Image*>::at(pos+1)=image;
}

void Model::bringImageDown(Image *image)
{
    int pos=imageStackPosition(image);
    if (pos<=0)
        return;

    Image *v=std::vector<Image*>::at(pos-1);
    std::vector<Image*>::at(pos)=v;
    std::vector<Image*>::at(pos-1)=image;
}

void Model::setVolumeStackPosition(Volume *volume, unsigned int position)
{
    if (std::vector<Volume*>::empty())
        return;

    while (volumeStackPosition(volume)<position)
        bringVolumeUp(volume);

    while (volumeStackPosition(volume)>position)
        bringVolumeDown(volume);
}

void Model::setImageStackPosition(Image *image, unsigned int position)
{
    if (std::vector<Image*>::empty())
        return;

    while (imageStackPosition(image)<position)
        bringImageUp(image);

    while (imageStackPosition(image)>position)
        bringImageDown(image);
}

void Model::setModel1d(Model *model)
{
    _1dModel=model;
    if (_1dModel)
        _1dModel->_is1dModel=true;
}

std::vector<Point> Model::points() const
{
    return _points;
}

void Model::recreatePointsList()
{
    std::vector<Point> p;
    for (unsigned int i=0 ; i<std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=std::vector<Volume*>::at(i);
        for (unsigned int j=0 ; j<volume->size() ; j++)
        {
            Path *path=volume->at(j);

            Point b=path->begin();
            Point e=path->end();

            p.push_back(b);
            p.push_back(e);
        }
    }

    _points.clear();
    for (unsigned int i=0 ; i<p.size() ; i++)
    {
        Point &point=p.at(i);

        bool defined=false;
        for (unsigned int j=0 ; j<_points.size() ; j++)
        {
            Point &point2=_points.at(j);
            if (GeometryTools::equivalentPoints(point, point2))
            {
                defined=true;
                break;
            }
        }

        if (defined)
            continue;

        _points.push_back(point);
    }
}

En13370Plugin *Model::en13370Plugin() const
{
    return _en13370;
}

void Model::createEn13370Plugin()
{
    if (_en13370)
        return;
    _en13370=new En13370Plugin;
}

void Model::removeEn13370Plugin()
{
    if (_en13370)
    {
        LOG_INFO("Delete En 13370 plugin.");

        delete _en13370;
        _en13370=0;
    }
}

double Model::xMin(ExportViewUserZoomType type, bool showImages, bool showDxf, bool showAnnotations) const
{
    if (type==UserView1dModel && _1dModelUserView)
        return _1dModelXmin;
    if (type==UserView2dModel && _2dModelUserView)
        return _2dModelXmin;
    if (type==UserViewResults && _resultsUserView)
        return _resultsXmin;
    if (type==UserViewEnvs && _envsUserView)
        return _envsXmin;
    if (type==UserViewCondensation && _condensationUserView)
        return _condensationXmin;
    return xMin(showImages, showDxf, showAnnotations);
}

double Model::xMax(ExportViewUserZoomType type, bool showImages, bool showDxf, bool showAnnotations) const
{
    if (type==UserView1dModel && _1dModelUserView)
        return _1dModelXmax;
    if (type==UserView2dModel && _2dModelUserView)
        return _2dModelXmax;
    if (type==UserViewResults && _resultsUserView)
        return _resultsXmax;
    if (type==UserViewEnvs && _envsUserView)
        return _envsXmax;
    if (type==UserViewCondensation && _condensationUserView)
        return _condensationXmax;
    return xMax(showImages, showDxf, showAnnotations);
}

double Model::yMin(ExportViewUserZoomType type, bool showImages, bool showDxf, bool showAnnotations) const
{
    if (type==UserView1dModel && _1dModelUserView)
        return _1dModelYmin;
    if (type==UserView2dModel && _2dModelUserView)
        return _2dModelYmin;
    if (type==UserViewResults && _resultsUserView)
        return _resultsYmin;
    if (type==UserViewEnvs && _envsUserView)
        return _envsYmin;
    if (type==UserViewCondensation && _condensationUserView)
        return _condensationYmin;
    return yMin(showImages, showDxf, showAnnotations);
}

double Model::yMax(ExportViewUserZoomType type, bool showImages, bool showDxf, bool showAnnotations) const
{
    if (type==UserView1dModel && _1dModelUserView)
        return _1dModelYmax;
    if (type==UserView2dModel && _2dModelUserView)
        return _2dModelYmax;
    if (type==UserViewResults && _resultsUserView)
        return _resultsYmax;
    if (type==UserViewEnvs && _envsUserView)
        return _envsYmax;
    if (type==UserViewCondensation && _condensationUserView)
        return _condensationYmax;
    return yMax(showImages, showDxf, showAnnotations);
}

void Model::use1dModelUserView(double xmin, double ymin, double xmax, double ymax)
{
    _1dModelUserView=true;
    _1dModelXmin=xmin;
    _1dModelXmax=xmax;
    _1dModelYmin=ymin;
    _1dModelYmax=ymax;
}

void Model::use1dModelUserView(bool use)
{
    _1dModelUserView=use;
}

void Model::use2dModelUserView(double xmin, double ymin, double xmax, double ymax)
{
    _2dModelUserView=true;
    _2dModelXmin=xmin;
    _2dModelXmax=xmax;
    _2dModelYmin=ymin;
    _2dModelYmax=ymax;
}

void Model::use2dModelUserView(bool use)
{
    _2dModelUserView=use;
}

void Model::useResultsUserView(double xmin, double ymin, double xmax, double ymax)
{
    _resultsUserView=true;
    _resultsXmin=xmin;
    _resultsXmax=xmax;
    _resultsYmin=ymin;
    _resultsYmax=ymax;
}

void Model::useResultsUserView(bool use)
{
    _resultsUserView=use;
}

void Model::useEnvsUserView(double xmin, double ymin, double xmax, double ymax)
{
    _envsUserView=true;
    _envsXmin=xmin;
    _envsXmax=xmax;
    _envsYmin=ymin;
    _envsYmax=ymax;
}

void Model::useEnvsUserView(bool use)
{
    _envsUserView=use;
}

void Model::useCondensationUserView(double xmin, double ymin, double xmax, double ymax)
{
    _condensationUserView = true;
    _condensationXmin = xmin;
    _condensationXmax = xmax;
    _condensationYmin = ymin;
    _condensationYmax = ymax;
}

void Model::useCondensationUserView(bool use)
{
    _condensationUserView = use;
}

void Model::addPolygon(const std::vector<double> x_values, const std::vector<double> y_values, const std::string &materialId)
{
    if (x_values.size()!=y_values.size())
        return;

    Volume *volume=new Volume;
    std::vector<Volume*>::push_back(volume);

    Material *material=Materials::instance()->materialFromId(materialId);
    volume->setMaterial(material);

    for (unsigned int i=0 ; i<x_values.size() ; i++)
    {
        double x0=x_values.at(i);
        double y0=y_values.at(i);

        double x1=x_values.at(0);
        double y1=y_values.at(0);

        if (i<x_values.size()-1)
        {
            x1=x_values.at(i+1);
            y1=y_values.at(i+1);
        }

        Line *line=new Line;

        Point begin;
        begin.setX(x0);
        begin.setY(y0);
        line->setBegin(begin, 0);

        Point end;
        end.setX(x1);
        end.setY(y1);
        line->setEnd(end, 0);

        volume->push_back(line);
    }

    volume->computeLines();
    volume->computeBounds();
}

void Model::finalizePolygons()
{
    recreateSurfaces();
}

void Model::addSurface(double x0, double y0, double x1, double y1, const std::string &boundaryId)
{
    BoundaryCondition *condition=BoundaryConditions::instance()->bcFromId(boundaryId);

    double x=0.5*(x0+x1);
    double y=0.5*(y0+y1);

    Surface *surface=getSurfaceAtPosition(x, y, 1.0e-6);
    if (surface)
        surface->setBoundaryCondition(condition);
}

Model::ModelType Model::modelType() const
{
    return _filetype;
}

void Model::setMaterialImportCategory(const std::string &category)
{
    _materialImportCategory=category;
}

Model::ComputationType Model::computationType() const
{
    return _computationType;
}

void Model::setComputationType(ComputationType type)
{
    _computationType=type;

    if (type==Flux && _1dModel)
    {
        delete _1dModel;
        _1dModel=0;
    }

    if (type==EquivalentThermalConductivity && _1dModel)
    {
        delete _1dModel;
        _1dModel=0;
    }

    if (type == GlaserCondensation && _1dModel)
    {
        delete _1dModel;
        _1dModel = 0;
    }

    else if (type!=Flux && !_1dModel && type!=EquivalentThermalConductivity)
    {
        _1dModel=new Model;
        _1dModel->_is1dModel=true;
    }
}

double Model::transmissionCoefficientLength() const
{
    if (_transmissionCoefficientLengthType==FullLength)
        return coefficientTransmissionFullLength();
    if (_transmissionCoefficientLengthType==ProjectedXLength)
        return coefficientTransmissionProjectedXLength();
    if (_transmissionCoefficientLengthType==ProjectedYLength)
        return coefficientTransmissionProjectedYLength();
    return _transmissionCoefficientLength;
}

void Model::setTransmissionCoefficientLength(double length)
{
    _transmissionCoefficientLength=length;
}

double Model::coefficientTransmissionFullLength() const
{
    double length=0.0;

    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);
        
        BoundaryCondition *bc=surface->boundaryCondition();
        if (!bc)
            continue;

        double flux=surface->flux();
        if (flux<=0.0)
            continue;

        length+=surface->length();
    }

    return length;
}

double Model::coefficientTransmissionProjectedXLength() const
{
    double length=0.0;

    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);

        if (!surface->isHorizontal())
            continue;
        
        BoundaryCondition *bc=surface->boundaryCondition();
        if (!bc)
            continue;

        double flux=surface->flux();
        if (flux<=0.0)
            continue;

        length+=surface->length();
    }

    return length;
}

double Model::coefficientTransmissionProjectedYLength() const
{
    double length=0.0;

    for (unsigned int i=0 ; i<std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=std::vector<Surface*>::at(i);

        if (!surface->isVertical())
            continue;
        
        BoundaryCondition *bc=surface->boundaryCondition();
        if (!bc)
            continue;

        double flux=surface->flux();
        if (flux<=0.0)
            continue;

        length+=surface->length();
    }

    return length;
}

Model::TransmissionCoefficientLengthType Model::transmissionCoefficientLengthType() const
{
    return _transmissionCoefficientLengthType;
}

void Model::setTransmissionCoefficientLengthType(TransmissionCoefficientLengthType type)
{
    _transmissionCoefficientLengthType=type;
}

bool Model::validResults() const
{
    return _validResults;
}

void Model::importInterop(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *model=doc.RootElement();
    if (!model)
    {
        setError(error, BadModelFileRootElement);
        return;
    }
}

std::string Model::exportInterop() const
{
    // Don't include results.
    if (_is1dModel)
        return "";

    std::string xml;
    xml+="<results>";

    if (_validResults)
    {
        // Computation type.
        xml+="<computation-type>";
        if (_computationType==ThermalBridge)
            xml+="thermal-bridge";
        else if (_computationType==Flux)
            xml+="thermal-flow";
        else if (_computationType==ThermalTransmission)
            xml+="u-coefficient";
        else if (_computationType == GlaserCondensation)
            xml+="condensation-glaser";
        else
            xml+="lambda-equivalent";
        xml+="</computation-type>";

        // Numerical results.
        xml+="<flux-2d>"+ToolBox::doubleToString(flux2d())+"</flux-2d>";

        if (_computationType==ThermalBridge || _computationType==ThermalTransmission)
            xml+="<flux-1d>"+ToolBox::doubleToString(flux1d())+"</flux-1d>";

        if (_computationType==ThermalBridge && psiCoefficientComputed())
            xml+="<thermal-bridge>"+ToolBox::doubleToString(psiCoefficient())+"</thermal-bridge>";

        if (_computationType==ThermalTransmission)
            xml+="<u-coefficient>"+ToolBox::doubleToString(transmissionCoefficient())+"</u-coefficient>";

        if (_computationType==EquivalentThermalConductivity)
            xml+="<lambda-equivalent>" + ToolBox::doubleToString(equivalentThermalConductivity()) + "</lambda-equivalent>";

        if (_computationType==ThermalBridge)
        {
            std::vector<EnvironmentResult> environments=environmentResults();
            for (unsigned int i=0 ; i<environments.size() ; i++)
            {
                std::string id1=environments.at(i)._environment1Id;
                std::string id2=environments.at(i)._environment2Id;
                double value=environments.at(i)._couplingCoefficient;

                if (id1=="1" && id2=="2")
                    xml+="<psi-12>"+ToolBox::doubleToString(value)+"</psi-12>";
                else if (id1=="1" && id2=="3")
                    xml+="<psi-13>"+ToolBox::doubleToString(value)+"</psi-13>";
                else if (id1=="1" && id2=="4")
                    xml+="<psi-14>"+ToolBox::doubleToString(value)+"</psi-14>";
                else if (id1=="2" && id2=="3")
                    xml+="<psi-23>"+ToolBox::doubleToString(value)+"</psi-23>";
                else if (id1=="2" && id2=="4")
                    xml+="<psi-24>"+ToolBox::doubleToString(value)+"</psi-24>";
                else if (id1=="3" && id2=="4")
                    xml+="<psi-34>"+ToolBox::doubleToString(value)+"</psi-34>";
            }

            // Global coupling coefficients.
            for (int i=1 ; i<=4 ; i++)
            {
                std::string id = ToolBox::intToString(i);

                if (!ToolBox::useEnvironment(environments, id))
                    continue;

                double value = ToolBox::globalCouplingCoefficient(environments, id);

                xml+="<psi-" + id + ">" + ToolBox::doubleToString(value) + "</psi-" + id + ">";
            }
        }

        // Images.
        xml+="<view-2d>"+_2dPngView+"</view-2d>";

        if (_computationType==ThermalBridge || _computationType==ThermalTransmission)
            xml+="<view-1d>"+_1dPngView+"</view-1d>";

        xml+="<results-2d>"+_2dResultsPngView+"</results-2d>";

        if (_computationType==ThermalBridge || _computationType==ThermalTransmission)
            xml+="<results-1d>"+_1dResultsPngView+"</results-1d>";
    }

    xml+="</results>";

    return xml;
}

bool Model::getCondensation() const
{
    return _condensation;
}

void Model::setCondensation(bool condensation)
{
    _condensation = condensation;
}

double Model::getEquivalentConductivityResistanceSum() const
{
    return _equivalentConductivityResistanceSum;
}

void Model::setEquivalentConductivityResistanceSum(double equivalentConducitityResistanceSum)
{
    _equivalentConductivityResistanceSum = equivalentConducitityResistanceSum;
}

double Model::getEquivalentConductivityThickness() const
{
    return _equivalentConductivityThickness;
}

void Model::setEquivalentConductivityThickness(double equivalentConductivityThickness)
{
    _equivalentConductivityThickness = equivalentConductivityThickness;
}

double Model::getEquivalentConductivityWidth() const
{
    return _equivalentConductivityWidth;
}

void Model::setEquivalentConductivityWidth(double equivalentConductivityWidth)
{
    _equivalentConductivityWidth = equivalentConductivityWidth;
}

void Model::set2dPngView(const std::string &image)
{
    _2dPngView=image;
}

void Model::set1dPngView(const std::string &image)
{
    _1dPngView=image;
}

void Model::set2dResultsPngView(const std::string &image)
{
    _2dResultsPngView=image;
}

void Model::set1dResultsPngView(const std::string &image)
{
    _1dResultsPngView=image;
}

void Model::setResultsValidity(bool valid)
{
    _validResults=valid;
}

bool Model::isEmpty() const
{
    return std::vector<Volume*>::empty();
}

std::string Model::exportEnvironments() const
{
    std::string xml = "<environments>";

    Environment *env1 = Environments::instance()->environmentFromName("1");
    if (env1)
    {
        std::string name = env1->description();
        name = ToolBox::escapeXmlCharacters(name);
        xml += "<environment id=\"1\">" + name + "</environment>";
    }

    Environment *env2 = Environments::instance()->environmentFromName("2");
    if (env2)
    {
        std::string name = env2->description();
        name = ToolBox::escapeXmlCharacters(name);
        xml += "<environment id=\"2\">" + name + "</environment>";
    }

    Environment *env3 = Environments::instance()->environmentFromName("3");
    if (env3)
    {
        std::string name = env3->description();
        name = ToolBox::escapeXmlCharacters(name);
        xml += "<environment id=\"3\">" + name + "</environment>";
    }

    Environment *env4 = Environments::instance()->environmentFromName("4");
    if (env4)
    {
        std::string name = env4->description();
        name = ToolBox::escapeXmlCharacters(name);
        xml += "<environment id=\"4\">" + name + "</environment>";
    }

    xml += "</environments>";

    return xml;
}

void Model::importEnvironments(const std::string &xml)
{
    // Init.
    Environments::instance()->initializeDescriptions();

    // Read Xml.
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *model=doc.RootElement();
    if (!model)
        return;

    TiXmlElement *environments = model->FirstChildElement("environments");
    if (!environments)
        return;

    TiXmlElement *environment = environments->FirstChildElement("environment");

    while (environment)
    {
        // Read id.
        const char *id = environment->Attribute("id");

        // Read description.
        const char *description = environment->GetText();

        if (id && description)
        {
            std::string s_id   = id;
            std::string s_desc = description;

            if (!s_desc.empty())
            {
                Environment *env = Environments::instance()->environmentFromName(s_id);
                if (env)
                    env->setDescription(s_desc);
            }
        }

        environment = environment->NextSiblingElement("environment");
    }
}
