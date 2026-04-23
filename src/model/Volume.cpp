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

#include <model/Volume.h>
#include <geometry/transformations/Rotation.h>
#include <geometry/GeometryTools.h>
#include <errors/ErrorHandler.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <geometry/Path.h>
#include <geometry/Line.h>
#include <tools/ToolBox.h>
#include <model/Surface.h>
#include <geometry/Arc.h>
#include <numeric/Cell.h>
#include <model/Model.h>
#include <tinyxml.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

Volume::Volume():
    _cellSize(0.025),
    _selected(false),
    _refinment(1.0),
    _id(ToolBox::getNewUuuid())
{
}

Volume::Volume(const Volume &volume)
{
    copyFrom(volume);
}

void Volume::setId(const std::string &id)
{
    _id=id;
}

std::string Volume::id() const
{
    return _id;
}

void Volume::operator=(const Volume &volume)
{
    copyFrom(volume);
}

void Volume::copyFrom(const Volume &volume)
{
    _materialId=volume._materialId;
    _cellSize=volume._cellSize;
    _id=volume._id;
    _internalVolumes=volume._internalVolumes;
    _internalCells=volume._internalCells;
    _selected=false;

    _xmin=volume._xmin;
    _xmax=volume._xmax;
    _ymin=volume._ymin;
    _ymax=volume._ymax;

    for (unsigned int i=0 ; i<volume.size() ; i++)
    {
        Path *path=volume.at(i);
        if (path->type()==Path::LinePath)
        {
            Line *line=(Line*) path;
            Line *copy=new Line(*line);
            push_back(copy);
        }
        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            Arc *copy=new Arc(*arc);
            push_back(copy);
        }
    }
}

Volume::~Volume()
{
    clean();
}

void Volume::clean()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
    resize(0);
}

bool Volume::check(ErrorHandler *error)
{
    Material *m=Materials::instance()->materialFromId(_materialId);
    if (m && !m->check(error))
        return false;

    if (_cellSize<=0.0)
    {
        setError(error, BadCellHeight);
        return false;
    }
    
    if (!isClosed())
    {
        setError(error, VolumeNotClosed);
        return false;
    }

    if (isSelfIntersected())
    {
        setError(error, VolumeSelfIntersection);
        return false;
    }

    return true;
}

void Volume::setMaterial(Material *material)
{
    if (!material)
        _materialId="";
    else 
        _materialId=material->id();
}

void Volume::setCellSize(double size, ErrorHandler *error)
{
    if (size<=0.0)
    {
        setError(error, BadCellWidth);
        return;
    }
    _cellSize=size;
}

Material *Volume::material() const
{
    return Materials::instance()->materialFromId(_materialId);
}

double Volume::cellsize() const
{
    return _cellSize;
}

std::string Volume::serialize() const
{
    std::string xml="<volume>";

    Material *m=Materials::instance()->materialFromId(_materialId);
    if (m)
        xml+="<material ref=\""+m->id()+"\" />";

    xml+="<cells>";
    xml+="<cell-size>"+ToolBox::doubleToString(_cellSize)+"</cell-size>";
    xml+="</cells>";

    xml+="<paths>";
    for (unsigned int i=0 ; i<size() ; i++)
        xml+=at(i)->serialize();
    xml+="</paths>";

    xml+="</volume>";
    return xml;
}

std::string Volume::serializeAsRectangle() const
{
    double x_min=xMin();
    double x_max=xMax();
    double y_min=yMin();
    double y_max=yMax();

    double height=y_max-y_min;

    std::string xml="<volume>";

    xml+="<x>"+ToolBox::doubleToString(x_min)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(-y_min-height)+"</y>";

    xml+="<width>"+ToolBox::doubleToString(x_max-x_min)+"</width>";
    xml+="<height>"+ToolBox::doubleToString(height)+"</height>";

    if (material())
        xml+="<conductivity ref=\""+material()->id()+"\">"+ToolBox::doubleToString(material()->conductivity())+"</conductivity>";
    xml+="<cells>";
    xml+="<max-width>"+ToolBox::doubleToString(cellsize())+"</max-width>";
    xml+="<max-height>"+ToolBox::doubleToString(cellsize())+"</max-height>";
    xml+="</cells>";

    xml+="</volume>";
    return xml;
}

Volume *Volume::createFromXml(const std::string &xml, ErrorHandler *error, std::map<std::string, std::string> &swap)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *volume=doc.RootElement();
    if (!volume)
    {
        setStaticError(error, BadVolumeNode);
        return 0;
    }

    if (!volume->Value())
    {
        setStaticError(error, BadVolumeNode);
        return 0;
    }

    std::string root_name=volume->Value();
    if (root_name!="volume")
    {
        setStaticError(error, BadVolumeNode);
        return 0;
    }

    Volume *v=new Volume;

    // Material.
    TiXmlElement *material_n=volume->FirstChildElement("material");
    if (material_n && material_n->Attribute("ref"))
    {
        std::string material_ref=material_n->Attribute("ref");
        Material *m=Materials::instance()->materialFromId(material_ref);

        std::string swap_material=swap[material_ref];
        if (!swap_material.empty())
            m=Materials::instance()->materialFromId(swap_material);

        v->setMaterial(m);
    }

    // Cells.
    TiXmlElement *cells=volume->FirstChildElement("cells");
    if (cells)
    {
        TiXmlElement *cells_size=cells->FirstChildElement("cell-size");
        if (cells_size && cells_size->GetText())
            v->setCellSize(ToolBox::convertStringToDouble(cells_size->GetText()), error);
    }

    // Paths.
    TiXmlElement *paths=volume->FirstChildElement("paths");
    if (paths)
    {
        TiXmlElement *path=paths->FirstChildElement();
        while (path)
        {
            std::string path_s=ToolBox::domToString(path);
            Path *p=Path::createFromXml(path_s, error);
            if (p)
                v->push_back(p);
            path=path->NextSiblingElement();
        }
    }

    v->computeBounds();

    return v;
}

bool Volume::isClosed_1() const
{
    // Volume is closed if number of surfaces & number of unique points
    // are the same.
    unsigned int n_surfaces=size();
    std::vector<Point> points;

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        
        Point begin=path->begin();
        Point end=path->end();

        points.push_back(begin);
        points.push_back(end);
    }

    std::vector<Point> points2;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point &p1=points.at(i);
        bool defined=false;
        for (unsigned int j=0 ; j<points2.size() ; j++)
        {
            Point &p2=points2.at(j);
            if (GeometryTools::equivalentPoints(p1, p2))
            {
                defined=true;
                break;
            }
        }
        if (!defined)
            points2.push_back(p1);
    }

    return (points2.size()==n_surfaces);
}

bool Volume::isClosed_2() const
{
    if (size()<=2)
        return false;

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        Point end=path->end();

        Path *next=at(0);
        if (i<size()-1)
            next=at(i+1);

        Point begin=next->begin();

        if (!GeometryTools::equivalentPoints(end, begin))
            return false;
    }

    return true;
}

bool Volume::isClosed() const
{
    if (isClosed_1())
        return true;
    if (isClosed_2())
        return true;
    return false;
}

bool Volume::isSelfIntersected() const
{
    // Volume is supposed not self-intersected if for any path composing it,
    // any each other path intersect whith it.
    std::vector<Line *> lines;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        if (path->type()==Path::LinePath)
            lines.push_back((Line*) path);

        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            for (unsigned int j=0 ; j<arc->size() ; j++)
                lines.push_back(arc->at(j));
        }
    }

    if (lines.size()<=1)
        return false;

    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        for (unsigned int j=i+1 ; j<lines.size() ; j++)
        {
            if (GeometryTools::intersectsWithoutBounds(lines.at(i), lines.at(j)))
                return true;
        }
    }

    return false;
}

bool Volume::containsPoint(const Point &point) const
{
    // If point is a volume defined point, return false.
    if (pointOnCorner(point))
        return false;

    double xmin=xMin();
    double xmax=xMax();
    double ymin=yMin();
    double ymax=yMax();

    Point pmin;
    pmin.setX(xmin);
    pmin.setY(ymin);

    Point pmax;
    pmax.setX(xmax);
    pmax.setY(ymax);

    double dx=pmax.x()-pmin.x();
    double dy=pmax.y()-pmin.y();
    double d=std::max(dx, dy);

    double theta=rand()/double(RAND_MAX)*2.0*M_PI;

    Point T;
    T.setX(pmin.x()+0.5*dx+5.0*d*cos(theta));
    T.setY(pmin.y()+0.5*dy+5.0*d*sin(theta));

    Line segment;
    segment.setBegin(point, 0);
    segment.setEnd(T, 0);

    int n=0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path::Type type=at(i)->type();
        if (type==Path::LinePath)
        {
            Line *line=(Line *) at(i);

            if (GeometryTools::intersects(&segment, line))
            {
                std::vector<Point> p=GeometryTools::intersectionPoint(&segment, line);
                n+=p.size();

                // Point is on a volume corner: cancel et restart.
                for (unsigned int j=0 ; j<p.size() ; j++)
                    if (pointOnCorner(p.at(j)))
                        return containsPoint(point);
            }
        }
        else if (type==Path::ArcPath)
        {
            Arc *arc=(Arc *) at(i);

            std::vector<Point> p=GeometryTools::intersectionPoint(&segment, arc);
            n+=p.size();

            // Point is on a volume corner: cancel et restart.
            for (unsigned int j=0 ; j<p.size() ; j++)
                if (pointOnCorner(p.at(j)))
                    return containsPoint(point);
        }
    }

    if (n%2==0)
        return false;
    return true;
}

bool Volume::pointOnCorner(const Point &point) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Point p1=at(i)->begin();
        Point p2=at(i)->end();

        if (GeometryTools::equivalentPoints(p1, point))
            return true;
        if (GeometryTools::equivalentPoints(p1, point))
            return true;
    }
    return false;
}

bool Volume::containsPath(Path *path) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *p=at(i);
        if (p->containsPath(path))
            return true;
    }
    return false;
}

bool Volume::pointOnBoundaries(const Point &point) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        Path::Type type=path->type();
        
        if (type==Path::LinePath)
        {
            Line *line=(Line *) path;
            if (line->containsPoint(point))
                return true;
        }

        else if (type==Path::ArcPath)
        {
            Arc *arc=(Arc *) path;

            if (arc->containsPoint(point))
                return true;
        }
    }
    return false;
}

void Volume::computeBounds()
{
    _xmin=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->xMin()<_xmin || i==0)
            _xmin=at(i)->xMin();
    }

    _xmax=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->xMax()>_xmax || i==0)
            _xmax=at(i)->xMax();
    }

    _ymin=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->yMin()<_ymin || i==0)
            _ymin=at(i)->yMin();
    }
    
    _ymax=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->yMax()>_ymax || i==0)
            _ymax=at(i)->yMax();
    }
}

double Volume::xMin() const
{
    return _xmin;
}

double Volume::xMax() const
{
    return _xmax;
}

double Volume::yMin() const
{
    return _ymin;
}

double Volume::yMax() const
{
    return _ymax;
}

Line *Volume::nearLine(const Point &p)
{
    Line *l=0;
    double distance=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        // Copy path.
        Path *path=at(i);
        Path::Type type=path->type();
        
        if (type==Path::LinePath)
        {
            Line *line=(Line *) path;
            double d=GeometryTools::pointDistance(line, p);
            if (d<distance || i==0)
            {
                distance=d;
                l=line;
            }
        }
    }
    return l;
}

std::vector<Line*> Volume::getLines() const
{
    std::vector<Line*> lines;

    for (unsigned int i=0 ; i<size() ; i++)
    {
        // Copy path.
        Path *path=at(i);
        Path::Type type=path->type();
        
        if (type==Path::LinePath)
        {
            Line *line=(Line *) path;
            Line *line2=new Line(*line);
            lines.push_back(line2);
        }

        else if (type==Path::ArcPath)
        {
            Arc *arc=(Arc *) path;
            for (unsigned int j=0 ; j<arc->size() ; j++)
            {
                Line *line=arc->at(j);
                Line *line2=new Line(*line);
                line2->setInitialPath(arc);
                lines.push_back(line2);
            }
        }
    }
    return lines;
}

void Volume::setSelected(bool selected)
{
    _selected=selected;
}

bool Volume::selected() const
{
    return _selected;
}

void Volume::translate(double dx, double dy)
{
    for (unsigned int i=0 ; i<size() ; i++)
        at(i)->translate(dx, dy);
    computeBounds();
}

void Volume::translatePoint(const Point &point, double dx, double dy)
{
    Point point2;
    point2.setX(point.x()+dx);
    point2.setY(point.y()+dy);

    // Do not apply transformation if point defines an arc.
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        if (path->type()!=Path::ArcPath)
            continue;

        Point b=path->begin();
        if (GeometryTools::equivalentPoints(b, point))
            return;

        Point e=path->end();
        if (GeometryTools::equivalentPoints(e, point))
            return;
    }

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        
        Point b=path->begin();
        if (GeometryTools::equivalentPoints(b, point))
            path->setBegin(point2, 0);

        Point e=path->end();
        if (GeometryTools::equivalentPoints(e, point))
            path->setEnd(point2, 0);
    }
}

void Volume::translateBorder(const Point &p1, const Point &p2, double dx, double dy)
{
    Point np1;
    np1.setX(p1.x()+dx);
    np1.setY(p1.y()+dy);

    Point np2;
    np2.setX(p2.x()+dx);
    np2.setY(p2.y()+dy);

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);

        Point b=path->begin();
        Point e=path->end();
        if (GeometryTools::equivalentPoints(b, p1) && GeometryTools::equivalentPoints(e, p2))
            path->translate(dx, dy);
        else if (GeometryTools::equivalentPoints(b, p2) && GeometryTools::equivalentPoints(e, p1))
            path->translate(dx, dy);
        else if (GeometryTools::equivalentPoints(b, p1))
            path->setBegin(np1, 0);
        else if (GeometryTools::equivalentPoints(b, p2))
            path->setBegin(np2, 0);
        else if (GeometryTools::equivalentPoints(e, p1))
            path->setEnd(np1, 0);
        else if (GeometryTools::equivalentPoints(e, p2))
            path->setEnd(np2, 0);
    }
}

void Volume::rotate(double x, double y, double angle)
{
    Point m;
    m.setX(x);
    m.setY(y);

    Rotation rotation;
    rotation.setCenter(m);
    rotation.setAngle(angle);

    // Rotate all points.
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        Point begin=path->begin();
        begin=rotation.apply(begin);

        Point end=path->end();
        end=rotation.apply(end);

        path->setBegin(begin, 0);
        path->setEnd(end, 0);

        Arc *arc=dynamic_cast<Arc*>(path);
        if (arc)
        {
            arc->setCenter(rotation.apply(arc->center()));
            arc->setRotationAngle(arc->rotationAngle()+angle);
            arc->recreateLines();
        }
    }
}

Point Volume::middle() const
{
    double xmin=xMin();
    double xmax=xMax();
    double ymin=yMin();
    double ymax=yMax();

    Point m;
    m.setX(0.5*(xmin+xmax));
    m.setY(0.5*(ymin+ymax));

    return m;
}

std::vector<Cell*> Volume::createCells(bool humidityMode, ErrorHandler *error) const
{
    // Create mesh.
    std::vector<Cell*> mesh;

    // No material.
    Material *m=Materials::instance()->materialFromId(_materialId);
    if (!m)
        return mesh;

    double conductivity=m->conductivity();

    if (humidityMode)
        conductivity = 1.0 / m->mu();

    double xmin=xMin();
    double xmax=xMax();
    double ymin=yMin();
    double ymax=yMax();

    double dx=xmax-xmin;
    double dy=ymax-ymin;
    double delta=std::min(dx, dy);
    delta=std::min(delta, _cellSize);
    delta=delta/_refinment;

    if (delta==0.0)
        return mesh;

    unsigned int airCellType=0;
    if (m->id()=="106D6720-211C-4090-8C60-7CED671C8580")
        airCellType=1;
    else if (m->id()=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
        airCellType=2;

    // Create all cells.
    int nx=int(dx/delta)+1;
    int ny=int(dy/delta)+1;

    for (int j=0 ; j<ny ; j++)
    {
        for (int i=0 ; i<nx ; i++)
        {
            Cell *cell=new Cell;
            cell->create(xmin+i*delta, ymin+j*delta, delta);
            cell->setHorizontalConductivity(conductivity);
            cell->setVerticalConductivity(conductivity);
            cell->setMaterial(material());
            cell->computeBounds();
            cell->_airCellType=airCellType;
            mesh.push_back(cell);
        }
    }

    // Define cells neighbors.
    for (int j=0 ; j<ny ; j++)
    {
        for (int i=0 ; i<nx ; i++)
        {
            unsigned int I=i+j*nx;
            Cell *cell=mesh.at(I);
            
            // Left.
            if (i>0)
                cell->_neighbor1=mesh.at(I-1);

            // Right.
            if (i<nx-1)
                cell->_neighbor3=mesh.at(I+1);

            // Bottom.
            if (j>0)
                cell->_neighbor2=mesh.at(i+(j-1)*nx);

            // Top.
            if (j<ny-1)
                cell->_neighbor4=mesh.at(i+(j+1)*nx);
        }
    }

    // Delete cells that are external to volume.
    std::vector<Cell*> mesh2;
    for (int j=0 ; j<ny ; j++)
    {
        for (int i=0 ; i<nx ; i++)
        {
            unsigned int I=i+j*nx;
            Cell *cell=mesh.at(I);
            bool add_cell=intersectionWithMesh(cell);
            if (!add_cell)
            {
                Point p;
                p.setX(xmin+(i+0.5)*delta);
                p.setY(ymin+(j+0.5)*delta);
                add_cell=containsPoint(p);
            }

            if (add_cell)
                mesh2.push_back(cell);
            else
                delete cell;
        }
    }
    
    return mesh2;
}

void Volume::computeLines()
{
    _lines.clear();
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        if (path->type()==Path::LinePath)
            _lines.push_back((Line*) path);

        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            for (unsigned int j=0 ; j<arc->size() ; j++)
                _lines.push_back(arc->at(j));
        }
    }
}

bool Volume::intersectionWithMesh(Cell *cell) const
{
    for (unsigned int i=0 ; i<cell->size() ; i++)
    {
        Line *line=cell->at(i);
        for (unsigned int j=0 ; j<_lines.size() ; j++)
        {
            if (GeometryTools::intersects(line, _lines.at(j)))
            {
                return true;
            }
        }
    }
    return false;
}

void Volume::initRefinment()
{
    _refinment=1.0;
}

void Volume::increaseRefinment()
{
    _refinment=_refinment*2.0;
}

void Volume::setModelForInternalVolumes(Model *model)
{
    _internalVolumes.clear();

    for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=model->std::vector<Volume*>::at(i);
        if (volume==this)
            continue;

        bool intersection=intersectionWithVolume(volume);
        bool addVolume=false;

        // Intersection: add to internal volume.
        if (intersection)
            addVolume=true;

        // No intersection: add to internal volume if first volume point is
        // inside current volume.
        if (!addVolume)
        {
            bool intoVolume=true;
            for (unsigned int j=0 ; j<volume->size() ; j++)
            {
                Path *path=volume->at(j);
                if (!containsPoint(path->begin()) && !pointOnBoundaries(path->begin()) && !pointOnCorner(path->begin()))
                {
                    intoVolume=false;
                    break;
                }
            }
            if (intoVolume)
                addVolume=true;
        }

        if (addVolume)
        {
            int current_idx=model->volumeStackPosition(this);
            int other_idx=model->volumeStackPosition(volume);
            if (other_idx>current_idx)
                _internalVolumes.push_back(volume);
        }
    }
}

bool Volume::intersectionWithVolume(Volume *volume) const
{
    // Current volume.
    std::vector<Line*> lines;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        if (path->type()==Path::LinePath)
            lines.push_back((Line*) path);

        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            for (unsigned int j=0 ; j<arc->size() ; j++)
                lines.push_back(arc->at(j));
        }
    }

    // Other volume.
    std::vector<Line*> lines2;
    for (unsigned int i=0 ; i<volume->size() ; i++)
    {
        Path *path=volume->at(i);
        if (path->type()==Path::LinePath)
            lines2.push_back((Line*) path);

        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            for (unsigned int j=0 ; j<arc->size() ; j++)
                lines2.push_back(arc->at(j));
        }
    }

    if (lines.size()<=1)
        return false;
    if (lines2.size()<=1)
        return false;

    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        for (unsigned int j=0 ; j<lines2.size() ; j++)
        {
            if (GeometryTools::intersectsWithoutBounds(lines.at(i), lines2.at(j)))
                return true;
        }
    }

    return false;
}

bool Volume::isIntoCell(Cell *cell) const
{
    if (intersectionWithMesh(cell))
        return true;

    if (empty())
        return false;
    Path *path=at(0);

    if (cell->containsPoint(path->begin()))
        return true;
    
    return false;
}

void Volume::close()
{
    if (size()<=1)
        return;

    for (unsigned int i=0 ; i<size()-1 ; i++)
    {
        Path *line1=at(i);
        Path *old_line=at(i+1);
        Point end=line1->end();
        for (unsigned int j=i+1 ; j<size() ; j++)
        {
            Path *line2=at(j);
            Point b=line2->begin();
            Point e=line2->end();

            if (GeometryTools::equivalentPoints(end, b))
            {
                at(i+1)=line2;
                at(j)=old_line;
                break;
            }

            if (GeometryTools::equivalentPoints(end, e))
            {
                line2->setBegin(e, 0);
                line2->setEnd(b, 0);
                at(i+1)=line2;
                at(j)=old_line;
                break;
            }
        }
    }
}

std::vector<Volume*> Volume::split() const
{
    std::vector<Path*> lines3=*this;
    std::vector<Path*> lines2=*this;
    std::vector<Volume*> volumes;

    bool lines_defined=!lines2.empty();
    while (lines_defined)
    {
        // Get first not null line.
        int idx=-1;
        Path *l=firstLine(lines2, idx);
        if (!l)
            lines_defined=false;

        // Close cell.
        if (l)
        {
            std::vector<Path*> lines4=lines2;
            Volume *volume=new Volume(*this);
            volume->clean();

            if (l->type()==Path::LinePath)
            {
                Line *nl=new Line(*(Line*)l);
                volume->push_back(nl);
            }
            else if (l->type()==Path::ArcPath)
            {
                Arc *nl=new Arc(*(Arc*)l);
                volume->push_back(nl);
            }
            
            lines2.at(idx)=0;
            lines4.at(idx)=0;

            Path *l2=nextLine(l, lines4, idx);
            while (l2)
            {
                if (l2->type()==Path::LinePath)
                {
                    Line *nl=new Line(*(Line*)l2);
                    volume->push_back(nl);
                }
                else if (l2->type()==Path::ArcPath)
                {
                    Arc *nl=new Arc(*(Arc*)l2);
                    volume->push_back(nl);
                }
                lines2.at(idx)=0;
                lines4.at(idx)=0;
                l2=nextLine(l2, lines4, idx);
            }
            
            // Remove duplicates.
            std::vector<Path *> cell_lines=*volume;
            volume->resize(0);

            for (unsigned int j=0 ; j<cell_lines.size() ; j++)
            {
                Path *line1=cell_lines.at(j);
                if (!line1)
                    continue;

                for (unsigned int i=0 ; i<cell_lines.size() ; i++)
                {
                    if (i!=j)
                    {
                        Path *line2=cell_lines.at(i);
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
                    volume->push_back(cell_lines.at(j));
            
            volume->computeBounds();
            volumes.push_back(volume);
        }
    }
    
    return volumes;
}

Path *Volume::nextLine(Path *line, const std::vector<Path*> &lines, int &idx) const
{
    idx=-1;
    for (unsigned int i=0 ; i<lines.size() ; i++)
    {
        Path *line2=lines.at(i);
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

Path *Volume::firstLine(const std::vector<Path*> &lines, int &idx) const
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

bool Volume::containsArc() const
{
    for (unsigned int i=0 ; i<size() ; i++)
        if (at(i)->type()==Path::ArcPath)
            return true;
    return false;
}

bool Volume::isRectangle() const
{
    if (!isClosed())
        return false;
    if (isSelfIntersected())
        return false;

    if (size()!=4)
        return false;

    for (unsigned int i=0 ; i<size()-1 ; i++)
    {
        if (at(i)->type()!=Path::LinePath)
            return false;

        if (at(i+1)->type()!=Path::LinePath)
            return false;

        Line *line1=(Line*)at(i);
        Line *line2=(Line*)at(i+1);

        if (std::fabs(GeometryTools::scalarProduct(line1, line2))>1.0e-8)
            return false;
    }

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=(Line*)at(i);

        Point b=line->begin();
        Point e=line->end();

        double dx=e.x()-b.x();
        double dy=e.y()-b.y();

        if (std::fabs(dx*dy)>1.0e-9)
            return false;
    }

    return true;
}

void Volume::addPoint(const Point &point)
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);

        if (path->type()!=Path::LinePath)
            continue;

        Line *line=(Line*)at(i);

        if (!line->containsPoint(point))
            continue;

        Line *line1=new Line;
        line1->setBegin(line->begin(), 0);
        line1->setEnd(point, 0);

        Line *line2=new Line;
        line2->setBegin(point, 0);
        line2->setEnd(line->end(), 0);

        delete line;
        at(i)=line1;
        push_back(line2);
        close();

        return;
    }
}

void Volume::removePoint(const Point &point)
{
    std::vector<unsigned int> lines;

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);

        if (path->type()!=Path::LinePath)
            continue;

        Line *line=(Line*)at(i);

        Point b=line->begin();
        Point e=line->end();

        if (!GeometryTools::equivalentPoints(e, point) && ! GeometryTools::equivalentPoints(b, point))
            continue;

        lines.push_back(i);
    }
    
    if (lines.size()!=2)
        return;

    Line *line1=(Line*)at(lines.at(0));
    Line *line2=(Line*)at(lines.at(1));

    Point b;
    Point e;

    if (!GeometryTools::equivalentPoints(line1->begin(), point))
        b=line1->begin();
    else if (!GeometryTools::equivalentPoints(line2->begin(), point))
        b=line2->begin();

    if (!GeometryTools::equivalentPoints(line1->end(), point))
        e=line1->end();
    else if (!GeometryTools::equivalentPoints(line2->end(), point))
        e=line2->end();

    Line *line=new Line;
    line->setBegin(b, 0);
    line->setEnd(e, 0);

    push_back(line);

    delete line1;
    delete line2;

    unsigned int idx=std::max(lines.at(0), lines.at(1));
    erase(begin()+idx);

    idx=std::min(lines.at(0), lines.at(1));
    erase(begin()+idx);

    close();
}
