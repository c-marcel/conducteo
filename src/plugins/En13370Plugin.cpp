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

#include <plugins/En13370Plugin.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#endif

#ifdef __linux__
using namespace std;
#endif

En13370Plugin::En13370Plugin():
    _type(Median),
    _includeHorizontalInsulation(false),
    _includeVerticalInsulation(false),
    _floorWidth(0.0),
    _groundConductivity(2.0),
    _internalTemperature(20.0),
    _externalTemperature(0.0),
    _wallThickness(0.0),
    _floorThermalResistance(0.0),
    _floorInternalSuperficialResistance(0.0),
    _floorExternalSuperficialResistance(0.0),
    _underGroundDepth(0.0),
    _wallInternalSuperficialResistance(0.0),
    _wallExternalSuperficialResistance(0.0),
    _wallThermalResistance(0.0),
    _heatedUndergroundArea(0.0),
    _notHeatedUndergroundArea(0.0),
    _externalWallHeight(0.0),
    _externalWallResistance(0.0),
    _meanZoneHeight(0.0),
    _airExchangeRate(0.3),
    _zoneCeilResistance(0.0),
    _openingsRatio(0.0),
    _windSpeed(0.0),
    _windProtectionFactor(0.10),
    _insulationWidth(0.0),
    _insulationHeight(0.0),
    _horizontalInsulationResistance(0.0),
    _verticalInsulationResistance(0.0),
    _horizontalInsulationThickness(0.0),
    _verticalInsulationThickness(0.0),
    _modelFloorWidth(0.0),
    _useFloorWidth(true),
    _area(0.0),
    _perimeter(0.0),
    _useConductivity(true)
{
}

En13370Plugin::~En13370Plugin()
{
}

std::string En13370Plugin::name() const
{
    return "en-13370-plugin";
}

void En13370Plugin::setType(LowFloorType type)
{
    _type=type;
}

En13370Plugin::LowFloorType En13370Plugin::type() const
{
    return _type;
}

void En13370Plugin::includeHorizontalInsulation(bool include)
{
    _includeHorizontalInsulation=include;
}

bool En13370Plugin::includeHorizontalInsulation() const
{
    return _includeHorizontalInsulation;
}

void En13370Plugin::includeVerticalInsulation(bool include)
{
    _includeVerticalInsulation=include;
}

bool En13370Plugin::includeVerticalInsulation() const
{
    return _includeVerticalInsulation;
}

void En13370Plugin::setFloorWidth(double width)
{
    _floorWidth=width;
}

double En13370Plugin::floorWidth() const
{
    return _floorWidth;
}

void En13370Plugin::setModelFloorWidth(double width)
{
    _modelFloorWidth=width;
}

double En13370Plugin::modelFloorWidth() const
{
    return _modelFloorWidth;
}

void En13370Plugin::setGroundConductivity(double conductivity)
{
    _groundConductivity=conductivity;
}

double En13370Plugin::groundConductivity() const
{
    return _groundConductivity;
}

void En13370Plugin::setInternalTemperature(double temperature)
{
    _internalTemperature=temperature;
}

double En13370Plugin::internalTemperature() const
{
    return _internalTemperature;
}

void En13370Plugin::setExternalTemperature(double temperature)
{
    _externalTemperature=temperature;
}

double En13370Plugin::externalTemperature() const
{
    return _externalTemperature;
}

double En13370Plugin::getU0() const
{
    if (_type==Median)
        return getMedianU0();
    if (_type==CrawlSpace)
        return getCrawlSpaceU0();
    if (_type==HeatedUnderground)
        return getHeatedUndergroundU0();
    if (_type==NotHeatedUnderground)
        return getNotHeatedUndergroundU0();
    if (_type==PartiallyHeatedUnderground)
        return getPartiallyHeatedUndergroundU0();
    return 0.0;
}

double En13370Plugin::getDeltaUPsi() const
{
    double U1=0.0;
    double U2=0.0;
    double U3=0.0;

    if (_includeHorizontalInsulation)
        U2=getHorizontalInsulationU();

    if (_includeVerticalInsulation)
        U3=getVerticalInsulationU();

    U2=std::min(U2, U3);
    return std::min(U1, U2);
}

double En13370Plugin::getDeltaTemperature() const
{
    return _internalTemperature-_externalTemperature;
}

void En13370Plugin::setInternalEnvironment(const std::string &environment)
{
    _internalEnvironment=environment;
}

std::string En13370Plugin::internalEnvironment() const
{
    return _internalEnvironment;
}

void En13370Plugin::setExternalEnvironment(const std::string &environment)
{
    _externalEnvironment=environment;
}

std::string En13370Plugin::externalEnvironment() const
{
    return _externalEnvironment;
}

void En13370Plugin::setWallThickness(double thickness)
{
    _wallThickness=thickness;
}

double En13370Plugin::wallThickness() const
{
    return _wallThickness;
}

void En13370Plugin::setFloorResistance(double resistance)
{
    _floorThermalResistance=resistance;
}

double En13370Plugin::floorResistance() const
{
    return _floorThermalResistance;
}

void En13370Plugin::setFloorInternalSuperficialResistance(double resistance)
{
    _floorInternalSuperficialResistance=resistance;
}

double En13370Plugin::floorInternalSuperficialResistance() const
{
    return _floorInternalSuperficialResistance;
}

void En13370Plugin::setFloorExternalSuperficialResistance(double resistance)
{
    _floorExternalSuperficialResistance=resistance;
}

double En13370Plugin::floorExternalSuperficialResistance() const
{
    return _floorExternalSuperficialResistance;
}

void En13370Plugin::setUndergroundDepth(double depth)
{
    _underGroundDepth=depth;
}

double En13370Plugin::undergroundDepth() const
{
    return _underGroundDepth;
}

double En13370Plugin::getHorizontalInsulationU() const
{
    double dt=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);

    double Rp=_horizontalInsulationResistance-_horizontalInsulationThickness/_groundConductivity;
    double dp=Rp*_groundConductivity;

    double psi=-_groundConductivity/M_PI*(log(_insulationWidth/dt+1.0)-log(_insulationWidth/(dt+dp)+1.0));

    return 2.0*psi/_floorWidth;
}

double En13370Plugin::getVerticalInsulationU() const
{
    double dt=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);

    double Rp=_verticalInsulationResistance-_verticalInsulationThickness/_groundConductivity;
    double dp=Rp*_groundConductivity;

    double psi=-_groundConductivity/M_PI*(log(2.0*_insulationHeight/dt+1.0)-log(2.0*_insulationHeight/(dt+dp)+1.0));

    return 2.0*psi/_floorWidth;
}

void En13370Plugin::fromXml(const std::string &xml)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *plugin=doc.RootElement();
    if (!plugin)
        return;

    // Check plugin name.
    const char *c_name=plugin->Attribute("type");
    if (!c_name)
        return;

    if (std::string(c_name)!=name())
        return;

    // Get floor.
    TiXmlElement *n_floor=plugin->FirstChildElement("floor");
    if (!n_floor)
        return;

    TiXmlElement *n_width=n_floor->FirstChildElement("width");
    TiXmlElement *n_perimeter=n_floor->FirstChildElement("perimeter");
    TiXmlElement *n_area=n_floor->FirstChildElement("area");

    if (n_width)
    {
        _useFloorWidth=true;
        const char *c_width=n_width->GetText();
        if (c_width)
            _floorWidth=ToolBox::stringToDouble(c_width);
    }
    else if (n_perimeter && n_area)
    {
        _useFloorWidth=false;
        const char *c_perimeter=n_perimeter->GetText();
        const char *c_area=n_area->GetText();
        if (c_perimeter)
            _perimeter=ToolBox::stringToDouble(c_perimeter);
        if (c_area)
            _area=ToolBox::stringToDouble(c_area);

        _floorWidth = 2.0 * _area / _perimeter;
    }
    else
        return;

    TiXmlElement *n_model_width=n_floor->FirstChildElement("model-width");
    if (!n_model_width)
        return;
    if (!n_model_width->GetText())
        return;

    _modelFloorWidth=ToolBox::stringToDouble(n_model_width->GetText());

    // Get ground.
    TiXmlElement *n_ground=plugin->FirstChildElement("ground");
    if (!n_ground)
        return;

    TiXmlElement *n_conductivity=n_ground->FirstChildElement("conductivity");
    TiXmlElement *n_material=n_ground->FirstChildElement("material");

    if (n_conductivity)
    {
        _useConductivity=true;
        const char *c_conductivity=n_conductivity->GetText();
        if (c_conductivity)
            _groundConductivity=ToolBox::stringToDouble(c_conductivity);
    }
    else if (n_material)
    {
        _useConductivity=false;
        std::string s_material=ToolBox::domToString(n_material);
        Material *material=Material::createFromXml(s_material, 0);
        if (material)
        {
            _materialId=material->id();
            Material *m=Materials::instance()->materialFromId(_materialId);
            if (m)
                delete material;
            else
                Materials::instance()->addMaterialToUserDb(material);
        }
    }
    else
        return;

    // Get temperatures.
    TiXmlElement *n_environments=plugin->FirstChildElement("environments");
    if (!n_environments)
        return;

    TiXmlElement *n_int_temp=n_environments->FirstChildElement("internal-temperature");
    if (!n_int_temp)
        return;

    TiXmlElement *n_ext_temp=n_environments->FirstChildElement("external-temperature");
    if (!n_ext_temp)
        return;

    TiXmlElement *n_int_env=n_environments->FirstChildElement("internal-environment");
    if (!n_int_env)
        return;

    TiXmlElement *n_ext_env=n_environments->FirstChildElement("external-environment");
    if (!n_ext_env)
        return;

    const char *c_int_temp=n_int_temp->GetText();
    if (!c_int_temp)
        return;

    const char *c_ext_temp=n_ext_temp->GetText();
    if (!c_ext_temp)
        return;

    const char *c_int_env=n_int_env->GetText();
    if (!c_int_env)
        return;

    const char *c_ext_env=n_ext_env->GetText();
    if (!c_ext_env)
        return;

    _internalTemperature=ToolBox::stringToDouble(c_int_temp);
    _internalEnvironment=c_int_env;

    _externalTemperature=ToolBox::stringToDouble(c_ext_temp);
    _externalEnvironment=c_ext_env;

    // Common part.
    TiXmlElement *n_common=plugin->FirstChildElement("common");
    if (!n_common)
        return;

    TiXmlElement *n_wall_thickness=n_common->FirstChildElement("wall-thickness");
    if (!n_wall_thickness)
        return;

    TiXmlElement *n_floor_rsi=n_common->FirstChildElement("floor-rsi");
    if (!n_floor_rsi)
        return;

    TiXmlElement *n_floor_rse=n_common->FirstChildElement("floor-rse");
    if (!n_floor_rse)
        return;

    TiXmlElement *n_floor_r=n_common->FirstChildElement("floor-resistance");
    if (!n_floor_r)
        return;

    const char *c_wall_thickness=n_wall_thickness->GetText();
    if (!c_wall_thickness)
        return;

    const char *c_floor_rsi=n_floor_rsi->GetText();
    if (!c_floor_rsi)
        return;

    const char *c_floor_rse=n_floor_rse->GetText();
    if (!c_floor_rse)
        return;

    const char *c_floor_r=n_floor_r->GetText();
    if (!c_floor_r)
        return;

    _wallThickness=ToolBox::stringToDouble(c_wall_thickness);
    _floorInternalSuperficialResistance=ToolBox::stringToDouble(c_floor_rsi);
    _floorExternalSuperficialResistance=ToolBox::stringToDouble(c_floor_rse);
    _floorThermalResistance=ToolBox::stringToDouble(c_floor_r);

    // Type.
    TiXmlElement *n_type=plugin->FirstChildElement("type");
    if (!n_type)
        return;

    const char *c_type=n_type->GetText();
    if (!c_type)
        return;

    std::string s_type=c_type;
    if (s_type=="median")
        _type=Median;
    else if (s_type=="crawl")
        _type=CrawlSpace;
    else if (s_type=="heated-basement")
        _type=HeatedUnderground;
    else if (s_type=="unheated-basement")
        _type=NotHeatedUnderground;
    else if (s_type=="partially-heated-basement")
        _type=PartiallyHeatedUnderground;

    // Horizontal insulation.
    _includeHorizontalInsulation=false;

    TiXmlElement *n_h_insulation=n_common->FirstChildElement("horizontal-insulation");
    if (n_h_insulation)
    {
        TiXmlElement *n_thickness=n_h_insulation->FirstChildElement("thickness");
        TiXmlElement *n_resistance=n_h_insulation->FirstChildElement("resistance");
        TiXmlElement *n_length=n_h_insulation->FirstChildElement("length");

        if (n_thickness && n_resistance && n_length)
        {
            const char *c_thickness=n_thickness->GetText();
            const char *c_resistance=n_resistance->GetText();
            const char *c_length=n_length->GetText();

            if (c_thickness && c_resistance && c_length)
            {
                _includeHorizontalInsulation=true;
                _horizontalInsulationThickness=ToolBox::stringToDouble(c_thickness);
                _horizontalInsulationResistance=ToolBox::stringToDouble(c_resistance);
                _insulationWidth=ToolBox::stringToDouble(c_length);
            }
        }
    }

    // Vertical insulation.
    _includeVerticalInsulation=false;

    TiXmlElement *n_v_insulation=n_common->FirstChildElement("vertical-insulation");
    if (n_v_insulation)
    {
        TiXmlElement *n_thickness=n_v_insulation->FirstChildElement("thickness");
        TiXmlElement *n_resistance=n_v_insulation->FirstChildElement("resistance");
        TiXmlElement *n_length=n_v_insulation->FirstChildElement("length");

        if (n_thickness && n_resistance && n_length)
        {
            const char *c_thickness=n_thickness->GetText();
            const char *c_resistance=n_resistance->GetText();
            const char *c_length=n_length->GetText();

            if (c_thickness && c_resistance && c_length)
            {
                _includeVerticalInsulation=true;
                _verticalInsulationThickness=ToolBox::stringToDouble(c_thickness);
                _verticalInsulationResistance=ToolBox::stringToDouble(c_resistance);
                _insulationHeight=ToolBox::stringToDouble(c_length);
            }
        }
    }

    // Crawl.
    TiXmlElement *n_crawl=plugin->FirstChildElement("crawl");
    if (n_crawl)
    {
        TiXmlElement *n_height=n_crawl->FirstChildElement("height");
        TiXmlElement *n_ceil_r=n_crawl->FirstChildElement("ceil-resistance");
        TiXmlElement *n_ext_wall_r=n_crawl->FirstChildElement("external-wall-resistance");
        TiXmlElement *n_openings=n_crawl->FirstChildElement("openings-ratio");
        TiXmlElement *n_wind_speed=n_crawl->FirstChildElement("wind-speed");
        TiXmlElement *n_factor=n_crawl->FirstChildElement("wind-protection-factor");
        TiXmlElement *n_depth=n_crawl->FirstChildElement("depth");

        if (n_height && n_ceil_r && n_ext_wall_r && n_openings && n_wind_speed && n_factor && n_depth)
        {
            const char *c_height=n_height->GetText();
            const char *c_ceil_r=n_ceil_r->GetText();
            const char *c_ext_wall_r=n_ext_wall_r->GetText();
            const char *c_openings=n_openings->GetText();
            const char *c_wind_speed=n_wind_speed->GetText();
            const char *c_factor=n_factor->GetText();
            const char *c_depth=n_depth->GetText();

            if (c_height && c_ceil_r && c_ext_wall_r && c_openings && c_wind_speed && c_factor && c_depth)
            {
                _crawlHeight=ToolBox::stringToDouble(c_height);
                _zoneCeilResistance=ToolBox::stringToDouble(c_ceil_r);
                _externalWallResistance=ToolBox::stringToDouble(c_ext_wall_r);
                _openingsRatio=ToolBox::stringToDouble(c_openings);
                _windSpeed=ToolBox::stringToDouble(c_wind_speed);
                _windProtectionFactor=ToolBox::stringToDouble(c_factor);
                _underGroundDepth=ToolBox::stringToDouble(c_depth);

                // If crawl depth is greatter than 0.5 meters.
                if (_underGroundDepth>0.5)
                {
                    TiXmlElement *n_crawl_wall_rsi=n_crawl->FirstChildElement("crawl-wall-rsi");
                    TiXmlElement *n_crawl_wall_r=n_crawl->FirstChildElement("crawl-wall-resistance");
                    TiXmlElement *n_crawl_wall_rse=n_crawl->FirstChildElement("crawl-wall-rse");

                    if (n_crawl_wall_rsi && n_crawl_wall_r && n_crawl_wall_rse)
                    {
                        const char *c_crawl_wall_rsi=n_crawl_wall_rsi->GetText();
                        const char *c_crawl_wall_r=n_crawl_wall_r->GetText();
                        const char *c_crawl_wall_rse=n_crawl_wall_rse->GetText();

                        if (c_crawl_wall_rsi && c_crawl_wall_r && c_crawl_wall_rse)
                        {
                            _wallInternalSuperficialResistance=ToolBox::stringToDouble(c_crawl_wall_rsi);
                            _wallThermalResistance=ToolBox::stringToDouble(c_crawl_wall_r);
                            _wallExternalSuperficialResistance=ToolBox::stringToDouble(c_crawl_wall_rse);
                        }
                    }
                }
            }
        }
    }

    // Basement.
    TiXmlElement *n_basement=plugin->FirstChildElement("basement");
    if (n_basement)
    {
        TiXmlElement *n_depth=n_basement->FirstChildElement("depth");
        TiXmlElement *n_wall_rsi=n_basement->FirstChildElement("basement-wall-rsi");
        TiXmlElement *n_wall_r=n_basement->FirstChildElement("basement-wall-resistance");
        TiXmlElement *n_wall_rse=n_basement->FirstChildElement("basement-wall-rse");

        if (n_depth && n_wall_rsi && n_wall_r && n_wall_rse)
        {
            const char *c_depth=n_depth->GetText();
            const char *c_wall_rsi=n_wall_rsi->GetText();
            const char *c_wall_r=n_wall_r->GetText();
            const char *c_wall_rse=n_wall_rse->GetText();

            if (c_depth && c_wall_rsi && c_wall_r && c_wall_rse)
            {
                _underGroundDepth=ToolBox::stringToDouble(c_depth);
                _wallInternalSuperficialResistance=ToolBox::stringToDouble(c_wall_rsi);
                _wallThermalResistance=ToolBox::stringToDouble(c_wall_r);
                _wallExternalSuperficialResistance=ToolBox::stringToDouble(c_wall_rse);

                if (_type==NotHeatedUnderground || _type==PartiallyHeatedUnderground)
                {
                    TiXmlElement *n_ceil_r=n_basement->FirstChildElement("ceil-resistance");
                    TiXmlElement *n_mean_height=n_basement->FirstChildElement("mean-height");
                    TiXmlElement *n_rate=n_basement->FirstChildElement("air-exchange-rate");
                    TiXmlElement *n_wall_height=n_basement->FirstChildElement("external-wall-height");
                    TiXmlElement *n_wall_r=n_basement->FirstChildElement("external-wall-resistance");

                    if (n_ceil_r && n_mean_height && n_rate && n_wall_height && n_wall_r)
                    {
                        const char *c_ceil_r=n_ceil_r->GetText();
                        const char *c_mean_height=n_mean_height->GetText();
                        const char *c_rate=n_rate->GetText();
                        const char *c_wall_height=n_wall_height->GetText();
                        const char *c_wall_r=n_wall_r->GetText();

                        if (c_ceil_r && c_mean_height && c_rate && c_wall_height && c_wall_r)
                        {
                            _zoneCeilResistance=ToolBox::stringToDouble(c_ceil_r);
                            _meanZoneHeight=ToolBox::stringToDouble(c_mean_height);
                            _airExchangeRate=ToolBox::stringToDouble(c_rate);
                            _externalWallHeight=ToolBox::stringToDouble(c_wall_height);
                            _externalWallResistance=ToolBox::stringToDouble(c_wall_r);
                        }
                    }
                }

                if (_type==PartiallyHeatedUnderground)
                {
                    TiXmlElement *n_heated_area=n_basement->FirstChildElement("heated-area");
                    TiXmlElement *n_unheated_area=n_basement->FirstChildElement("unheated-area");

                    if (n_heated_area && n_unheated_area)
                    {
                        const char *c_heated_area=n_heated_area->GetText();
                        const char *c_unheated_area=n_unheated_area->GetText();

                        if (c_heated_area && c_unheated_area)
                        {
                            _heatedUndergroundArea=ToolBox::stringToDouble(c_heated_area);
                            _notHeatedUndergroundArea=ToolBox::stringToDouble(c_unheated_area);
                        }
                    }
                }
            }
        }
    }
}

bool En13370Plugin::useConductivity() const
{
    return _useConductivity;
}

std::string En13370Plugin::materialId() const
{
    return _materialId;
}

bool En13370Plugin::useFloorWidth() const
{
    return _useFloorWidth;
}

double En13370Plugin::perimeter() const
{
    return _perimeter;
}

double En13370Plugin::area() const
{
    return _area;
}

std::string En13370Plugin::toXml() const
{
    std::string xml;

    xml+="<plugin type=\""+name()+"\">";

    // Floor.
    xml+="<floor>";
    if (_useFloorWidth)
        xml+="<width>"+ToolBox::doubleToString(_floorWidth)+"</width>";
    else
    {
        xml+="<area>"+ToolBox::doubleToString(_area)+"</area>";
        xml+="<perimeter>"+ToolBox::doubleToString(_perimeter)+"</perimeter>";
    }
    xml+="<model-width>"+ToolBox::doubleToString(_modelFloorWidth)+"</model-width>";
    xml+="</floor>";

    // Ground.
    xml+="<ground>";
    Material *material=Materials::instance()->materialFromId(_materialId);
    if (!_useConductivity && material)
        xml+=material->serialize();
    else
        xml+="<conductivity>"+ToolBox::doubleToString(_groundConductivity)+"</conductivity>";
    xml+="</ground>";

    // Thermal environments.
    xml+="<environments>";
    xml+="<internal-temperature>"+ToolBox::doubleToString(_internalTemperature)+"</internal-temperature>";
    xml+="<external-temperature>"+ToolBox::doubleToString(_externalTemperature)+"</external-temperature>";
    xml+="<internal-environment>"+_internalEnvironment+"</internal-environment>";
    xml+="<external-environment>"+_externalEnvironment+"</external-environment>";
    xml+="</environments>";

    // Common part.
    xml+="<common>";
    xml+="<wall-thickness>"+ToolBox::doubleToString(_wallThickness)+"</wall-thickness>";
    xml+="<floor-rsi>"+ToolBox::doubleToString(_floorInternalSuperficialResistance)+"</floor-rsi>";
    xml+="<floor-resistance>"+ToolBox::doubleToString(_floorThermalResistance)+"</floor-resistance>";
    xml+="<floor-rse>"+ToolBox::doubleToString(_floorExternalSuperficialResistance)+"</floor-rse>";

    // horizontal insulation.
    if (_includeHorizontalInsulation)
    {
        xml+="<horizontal-insulation>";
        xml+="<thickness>"+ToolBox::doubleToString(_horizontalInsulationThickness)+"</thickness>";
        xml+="<resistance>"+ToolBox::doubleToString(_horizontalInsulationResistance)+"</resistance>";
        xml+="<length>"+ToolBox::doubleToString(_insulationWidth)+"</length>";
        xml+="</horizontal-insulation>";
    }

    // vertical insulation.
    if (_includeVerticalInsulation)
    {
        xml+="<vertical-insulation>";
        xml+="<thickness>"+ToolBox::doubleToString(_verticalInsulationThickness)+"</thickness>";
        xml+="<resistance>"+ToolBox::doubleToString(_verticalInsulationResistance)+"</resistance>";
        xml+="<length>"+ToolBox::doubleToString(_insulationHeight)+"</length>";
        xml+="</vertical-insulation>";
    }

    xml+="</common>";

    // Type.
    xml+="<type>";
    if (_type==Median)
        xml+="median";
    else if (_type==CrawlSpace)
        xml+="crawl";
    else if (_type==HeatedUnderground)
        xml+="heated-basement";
    else if (_type==NotHeatedUnderground)
        xml+="unheated-basement";
    else if (_type==PartiallyHeatedUnderground)
        xml+="partially-heated-basement";
    xml+="</type>";

    // Crawl.
    if (_type==CrawlSpace)
    {
        xml+="<crawl>";
        xml+="<height>"+ToolBox::doubleToString(_crawlHeight)+"</height>";
        xml+="<ceil-resistance>"+ToolBox::doubleToString(_zoneCeilResistance)+"</ceil-resistance>";
        xml+="<external-wall-resistance>"+ToolBox::doubleToString(_externalWallResistance)+"</external-wall-resistance>";
        xml+="<openings-ratio>"+ToolBox::doubleToString(_openingsRatio)+"</openings-ratio>";
        xml+="<wind-speed>"+ToolBox::doubleToString(_windSpeed)+"</wind-speed>";
        xml+="<wind-protection-factor>"+ToolBox::doubleToString(_windProtectionFactor)+"</wind-protection-factor>";
        xml+="<depth>"+ToolBox::doubleToString(_underGroundDepth)+"</depth>";

        // If crawl depth is greatter than 0.5 meters.
        if (_underGroundDepth>0.5)
        {
            xml+="<crawl-wall-rsi>"+ToolBox::doubleToString(_wallInternalSuperficialResistance)+"</crawl-wall-rsi>";
            xml+="<crawl-wall-resistance>"+ToolBox::doubleToString(_wallThermalResistance)+"</crawl-wall-resistance>";
            xml+="<crawl-wall-rse>"+ToolBox::doubleToString(_wallExternalSuperficialResistance)+"</crawl-wall-rse>";
        }

        xml+="</crawl>";
    }

    // Basement.
    else if (_type==HeatedUnderground || _type==NotHeatedUnderground || _type==PartiallyHeatedUnderground)
    {
        xml+="<basement>";
        xml+="<depth>"+ToolBox::doubleToString(_underGroundDepth)+"</depth>";
        xml+="<basement-wall-rsi>"+ToolBox::doubleToString(_wallInternalSuperficialResistance)+"</basement-wall-rsi>";
        xml+="<basement-wall-resistance>"+ToolBox::doubleToString(_wallThermalResistance)+"</basement-wall-resistance>";
        xml+="<basement-wall-rse>"+ToolBox::doubleToString(_wallExternalSuperficialResistance)+"</basement-wall-rse>";

        // Not heated or partially heated.
        if (_type==NotHeatedUnderground || _type==PartiallyHeatedUnderground)
        {
            xml+="<ceil-resistance>"+ToolBox::doubleToString(_zoneCeilResistance)+"</ceil-resistance>";
            xml+="<mean-height>"+ToolBox::doubleToString(_meanZoneHeight)+"</mean-height>";
            xml+="<air-exchange-rate>"+ToolBox::doubleToString(_airExchangeRate)+"</air-exchange-rate>";
            xml+="<external-wall-height>"+ToolBox::doubleToString(_externalWallHeight)+"</external-wall-height>";
            xml+="<external-wall-resistance>"+ToolBox::doubleToString(_externalWallResistance)+"</external-wall-resistance>";
        }

        // Partially heated.
        if (_type==PartiallyHeatedUnderground)
        {
            xml+="<heated-area>"+ToolBox::doubleToString(_heatedUndergroundArea)+"</heated-area>";
            xml+="<unheated-area>"+ToolBox::doubleToString(_notHeatedUndergroundArea)+"</unheated-area>";
        }
        
        xml+="</basement>";
    }

    xml+="</plugin>";

    return xml;
}

void En13370Plugin::useFloorWidth(bool use)
{
    _useFloorWidth=use;
}

void En13370Plugin::setPerimeter(double perimeter)
{
    _perimeter=perimeter;
}

void En13370Plugin::setArea(double area)
{
    _area=area;
}

double En13370Plugin::flux() const
{
    double f=getU0()*(modelFloorWidth()+getVerticalHeight())*getDeltaTemperature();
    if (isnan(f))
        return 0.0;
    if (isinf(f))
        return 0.0;
    return f;
}

double En13370Plugin::getVerticalHeight() const
{
    if (_type==Median)
        return 0.0;
    if (_type==CrawlSpace)
        return 0.0;
    return _underGroundDepth;
}

double En13370Plugin::flux(const std::string &environment1, const std::string &environment2) const
{
    if (environment1!=_internalEnvironment && environment1!=_externalEnvironment)
        return 0.0;
    if (environment2!=_internalEnvironment && environment2!=_externalEnvironment)
        return 0.0;
    if (environment1==environment2)
        return 0.0;
    double f=getU0()*(modelFloorWidth()+getVerticalHeight());

    if (isnan(f))
        return 0.0;
    if (isinf(f))
        return 0.0;
    return f;
}

double En13370Plugin::getMedianU0() const
{
    double dt=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);

    if (dt<_floorWidth)
        return 2.0*_groundConductivity/(M_PI*_floorWidth+dt)*log(M_PI*_floorWidth/dt+1.0)+getDeltaUPsi();
    
    return _groundConductivity/(0.457*_floorWidth+dt)+getDeltaUPsi();
}

void En13370Plugin::setWallInternalSuperficialResistance(double resistance)
{
    _wallInternalSuperficialResistance=resistance;
}

double En13370Plugin::wallInternalSuperficialResistance() const
{
    return _wallInternalSuperficialResistance;
}

void En13370Plugin::setWallExternalSuperficialResistance(double resistance)
{
    _wallExternalSuperficialResistance=resistance;
}

double En13370Plugin::wallExternalSuperficialResistance() const
{
    return _wallExternalSuperficialResistance;
}

void En13370Plugin::setWallResistance(double resistance)
{
    _wallThermalResistance=resistance;
}

double En13370Plugin::wallResistance() const
{
    return _wallThermalResistance;
}

double En13370Plugin::getCrawlSpaceU0() const
{
    double dg=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);
    double Ug=2.0*_groundConductivity/(M_PI*_floorWidth+dg)*log(M_PI*_floorWidth/dg+1.0);
    Ug=Ug+getDeltaUPsi();

    // Special case when crawl space is higher as 0.5 meter.
    if (_underGroundDepth>0.5)
    {
        double dt=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);

        // Undergound floor U.
        double Ubf=0.0;
        if (dt+0.5*_underGroundDepth<_floorWidth)
            Ubf=2.0*_groundConductivity/(M_PI*_floorWidth+dt+0.5*_underGroundDepth)*log(M_PI*_floorWidth/(dt+0.5*_underGroundDepth)+1.0);

        else
            Ubf=_groundConductivity/(0.457*_floorWidth+dt+0.5*_underGroundDepth);

        Ubf=Ubf+getDeltaUPsi();

        // Wall U.
        double dw=_groundConductivity*(_wallInternalSuperficialResistance+_wallThermalResistance+_wallExternalSuperficialResistance);

        if (dw<dt)
            dt=dw;

        double Ubw=2.0*_groundConductivity/(M_PI*_underGroundDepth)*(1.0+0.5*dt/(dt+_underGroundDepth))*log(_underGroundDepth/dw+1.0);

        Ug=Ubf+2.0*_underGroundDepth/_floorWidth*Ubw;
    }

    double Ux=2.0*_crawlHeight/(_externalWallResistance*_floorWidth)+1450.0*_openingsRatio*_windSpeed*_windProtectionFactor/_floorWidth;

    double R=_zoneCeilResistance+1.0/(Ug+Ux);

    return 1.0/R;
}

double En13370Plugin::getHeatedUndergroundU0() const
{
    double dt=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);

    // Undergound floor U.
    double Ubf=0.0;
    if (dt+0.5*_underGroundDepth<_floorWidth)
        Ubf=2.0*_groundConductivity/(M_PI*_floorWidth+dt+0.5*_underGroundDepth)*log(M_PI*_floorWidth/(dt+0.5*_underGroundDepth)+1.0);

    else
        Ubf=_groundConductivity/(0.457*_floorWidth+dt+0.5*_underGroundDepth);

    Ubf=Ubf+getDeltaUPsi();

    // Wall U.
    double dw=_groundConductivity*(_wallInternalSuperficialResistance+_wallThermalResistance+_wallExternalSuperficialResistance);

    if (dw<dt)
        dt=dw;

    double Ubw=2.0*_groundConductivity/(M_PI*_underGroundDepth)*(1.0+0.5*dt/(dt+_underGroundDepth))*log(_underGroundDepth/dw+1.0);

    return (_floorWidth*Ubf+2.0*_underGroundDepth*Ubw)/(_floorWidth+2.0*_underGroundDepth);
}

double En13370Plugin::getNotHeatedUndergroundU0() const
{
    double dt=_wallThickness+_groundConductivity*(_floorInternalSuperficialResistance+_floorThermalResistance+_floorExternalSuperficialResistance);

    // Undergound floor U.
    double Ubf=0.0;
    if (dt+0.5*_underGroundDepth<_floorWidth)
        Ubf=2.0*_groundConductivity/(M_PI*_floorWidth+dt+0.5*_underGroundDepth)*log(M_PI*_floorWidth/(dt+0.5*_underGroundDepth)+1.0);

    else
        Ubf=_groundConductivity/(0.457*_floorWidth+dt+0.5*_underGroundDepth);

    Ubf=Ubf+getDeltaUPsi();

    // Wall U.
    double dw=_groundConductivity*(_wallInternalSuperficialResistance+_wallThermalResistance+_wallExternalSuperficialResistance);

    if (dw<dt)
        dt=dw;

    double Ubw=2.0*_groundConductivity/(M_PI*_underGroundDepth)*(1.0+0.5*dt/(dt+_underGroundDepth))*log(_underGroundDepth/dw+1.0);

    double R=_zoneCeilResistance+_floorWidth/(_floorWidth*Ubf+2.0*_underGroundDepth*Ubw+2.0*_externalWallHeight/_externalWallResistance+0.33*_airExchangeRate*_floorWidth*_meanZoneHeight);
    return 1/R;
}

double En13370Plugin::getPartiallyHeatedUndergroundU0() const
{
    double Uh=getHeatedUndergroundU0();
    double Unh=getNotHeatedUndergroundU0();

    return (_heatedUndergroundArea*Uh+_notHeatedUndergroundArea*Unh)/(_heatedUndergroundArea+_notHeatedUndergroundArea);
}

void En13370Plugin::setHeatedUndergroundArea(double area)
{
    _heatedUndergroundArea=area;
}

double En13370Plugin::heatedUndergroundArea() const
{
    return _heatedUndergroundArea;
}

void En13370Plugin::setNotHeatedUndergroundArea(double area)
{
    _notHeatedUndergroundArea=area;
}

double En13370Plugin::notHeatedUndergroundArea() const
{
    return _notHeatedUndergroundArea;
}

void En13370Plugin::setExternalWallHeight(double height)
{
    _externalWallHeight=height;
}

double En13370Plugin::externalWellHeight() const
{
    return _externalWallHeight;
}

void En13370Plugin::setCrawlHeight(double height)
{
    _crawlHeight=height;
}

double En13370Plugin::crawlHeight() const
{
    return _crawlHeight;
}

void En13370Plugin::setExternalWallResistance(double resistance)
{
    _externalWallResistance=resistance;
}

double En13370Plugin::externalWallResistance() const
{
    return _externalWallResistance;
}

void En13370Plugin::setMeanZoneHeight(double height)
{
    _meanZoneHeight=height;
}

double En13370Plugin::meanZoneHeight() const
{
    return _meanZoneHeight;
}

void En13370Plugin::setAirExchangeRate(double rate)
{
    _airExchangeRate=rate;
}

double En13370Plugin::airExchangeRate() const
{
    return _airExchangeRate;
}

void En13370Plugin::setZoneCeilResistance(double resistance)
{
    _zoneCeilResistance=resistance;
}

double En13370Plugin::zoneCeilResistance() const
{
    return _zoneCeilResistance;
}

void En13370Plugin::setOpeningsRatio(double ratio)
{
    _openingsRatio=ratio;
}

double En13370Plugin::openingsRatio() const
{
    return _openingsRatio;
}

void En13370Plugin::setWindSpeed(double speed)
{
    _windSpeed=speed;
}

double En13370Plugin::windSpeed() const
{
    return _windSpeed;
}

void En13370Plugin::setWindProtectionFactor(double factor)
{
    _windProtectionFactor=factor;
}

double En13370Plugin::windProtectionFactor() const
{
    return _windProtectionFactor;
}

void En13370Plugin::setInsulationWidth(double width)
{
    _insulationWidth=width;
}

double En13370Plugin::insulationWidth() const
{
    return _insulationWidth;
}

void En13370Plugin::setInsulationHeight(double height)
{
    _insulationHeight=height;
}

double En13370Plugin::insulationHeight() const
{
    return _insulationHeight;
}

void En13370Plugin::setHorizontalInsulationResistance(double resistance)
{
    _horizontalInsulationResistance=resistance;
}

double En13370Plugin::horizontalInsulationResistance() const
{
    return _horizontalInsulationResistance;
}

void En13370Plugin::setVerticalInsulationResistance(double resistance)
{
    _verticalInsulationResistance=resistance;
}

double En13370Plugin::verticalInsulationResistance() const
{
    return _verticalInsulationResistance;
}

void En13370Plugin::setHorizontalInsulationThickness(double thickness)
{
    _horizontalInsulationThickness=thickness;
}

double En13370Plugin::horizontalInsulationThickness() const
{
    return _horizontalInsulationThickness;
}

void En13370Plugin::setVerticalInsulationThickness(double thickness)
{
    _verticalInsulationThickness=thickness;
}

double En13370Plugin::verticalInsulationThickness() const
{
    return _verticalInsulationThickness;
}

void En13370Plugin::useConductivity(bool use)
{
    _useConductivity=use;
}

void En13370Plugin::setMaterialId(const std::string &id)
{
    _materialId=id;
}
