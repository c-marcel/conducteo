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

#include <physics/BoundaryCondition.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>

double InfiniteResistance=-1.0;

BoundaryCondition::BoundaryCondition():
    _resistance(0.04),
    _temperature(0.0),
    _red(0),
    _green(0),
    _blue(0),
    _id(ToolBox::getNewUuuid()),
    _rh(0.8)
{
}

BoundaryCondition::~BoundaryCondition()
{
}

bool BoundaryCondition::check(ErrorHandler *error)
{
    if (_name.empty())
    {
        setError(error, BadBcName);
        return false;
    }

    if (_id.empty())
    {
        setError(error, BadBcId);
        return false;
    }

    if (_resistance!=InfiniteResistance && _resistance<=0.0)
    {
        setError(error, BadBcResistance);
        return false;
    }

    if (_rh < 0.0 || _rh > 1.0)
    {
        setError(error, BadBcRh);
        return false;
    }

    if (_red>255)
    {
        setError(error, BadBcColor);
        return false;
    }

    if (_green>255)
    {
        setError(error, BadBcColor);
        return false;
    }

    if (_blue>255)
    {
        setError(error, BadBcColor);
        return false;
    }

    return true;
}

void BoundaryCondition::setName(const std::string &name)
{
    _name=name;
    changed();
}

void BoundaryCondition::setDescription(const std::string &description)
{
    _description=description;
    changed();
}

void BoundaryCondition::setId(const std::string &id)
{
    _id=id;
    changed();
}

void BoundaryCondition::setResistance(double resistance, ErrorHandler *error)
{
    if (resistance<=0.0 && resistance!=InfiniteResistance)
    {
        setError(error, BadResistance);
        return;
    }
    _resistance=resistance;
    changed();
}

void BoundaryCondition::setTemperature(double temperature)
{
    _temperature=temperature;
    changed();
}

void BoundaryCondition::setColor(unsigned int red, unsigned int green, unsigned int blue)
{
    _red=red;
    _green=green;
    _blue=blue;
    changed();
}

std::string BoundaryCondition::name() const
{
    return _name;
}

std::string BoundaryCondition::description() const
{
    return _description;
}

std::string BoundaryCondition::id() const
{
    return _id;
}

double BoundaryCondition::resistance() const
{
    return _resistance;
}

double BoundaryCondition::temperature() const
{
    return _temperature;
}

unsigned int BoundaryCondition::red() const
{
    return _red;
}

unsigned int BoundaryCondition::green() const
{
    return _green;
}

unsigned int BoundaryCondition::blue() const
{
    return _blue;
}

std::string BoundaryCondition::serialize() const
{
    std::string xml="<boundary-condition id=\""+id()+"\" >";
    xml+="<name>"+ToolBox::escapeXmlCharacters(_name)+"</name>";
    xml+="<description>"+ToolBox::escapeXmlCharacters(_description)+"</description>";
    xml+="<temperature>"+ToolBox::doubleToString(_temperature)+"</temperature>";
    xml+="<resistance>"+ToolBox::doubleToString(_resistance)+"</resistance>";
    xml+="<humidity>"+ToolBox::doubleToString(_rh)+"</humidity>";
    xml+="<color>";
    xml+="<red>"+ToolBox::intToString(_red)+"</red>";
    xml+="<green>"+ToolBox::intToString(_green)+"</green>";
    xml+="<blue>"+ToolBox::intToString(_blue)+"</blue>";
    xml+="</color>";
    xml+="</boundary-condition>";
    return xml;
}

bool BoundaryCondition::isEqualTo(const BoundaryCondition *boundary) const
{
    if (!isEquivalentTo(boundary))
        return false;
    if (_id!=boundary->id())
        return false;
    if (_description!=boundary->description())
        return false;

    return true;
}

bool BoundaryCondition::isEquivalentTo(const BoundaryCondition *boundary) const
{
    if (_name!=boundary->name())
        return false;

    if (_resistance!=boundary->resistance())
        return false;
    if (_temperature!=boundary->temperature())
        return false;
    if (_rh!=boundary->rh())
        return false;

    if (_red!=boundary->red())
        return false;
    if (_green!=boundary->green())
        return false;
    if (_blue!=boundary->blue())
        return false;

    return true;
}

BoundaryCondition *BoundaryCondition::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *boundary=doc.RootElement();
    if (!boundary)
    {
        setStaticError(error, BadBcNode);
        return 0;
    }

    if (!boundary->Value())
    {
        setStaticError(error, BadBcNode);
        return 0;
    }

    std::string root_name=boundary->Value();
    if (root_name!="boundary-condition")
    {
        setStaticError(error, BadBcNode);
        return 0;
    }

    BoundaryCondition bc;

    if (boundary->Attribute("id"))
        bc.setId(boundary->Attribute("id"));
    else
    {
        setStaticError(error, BadBcId);
        return 0;
    }

    TiXmlElement *name=boundary->FirstChildElement("name");
    if (name && name->GetText())
        bc.setName(name->GetText());
    else
    {
        setStaticError(error, BadBcName);
        return 0;
    }

    TiXmlElement *description=boundary->FirstChildElement("description");
    if (description && description->GetText())
        bc.setDescription(description->GetText());

    TiXmlElement *resistance=boundary->FirstChildElement("resistance");
    if (resistance && resistance->GetText())
        bc.setResistance(ToolBox::convertStringToDouble(resistance->GetText()), error);
    else
    {
        setStaticError(error, BadBcResistance);
        return 0;
    }

    TiXmlElement *temperature=boundary->FirstChildElement("temperature");
    if (temperature && temperature->GetText())
        bc.setTemperature(ToolBox::convertStringToDouble(temperature->GetText()));
    else
    {
        if (bc.resistance()!=InfiniteResistance)
        {
            setStaticError(error, BadBcTemperature);
            return 0;
        }
    }

    TiXmlElement *humidity = boundary->FirstChildElement("humidity");
    if (humidity && humidity->GetText())
        bc.setRh(ToolBox::convertStringToDouble(humidity->GetText()), error);
    else
        bc.setRh(0.8, error);
    
    TiXmlElement *color=boundary->FirstChildElement("color");
    if (color)
    {
        TiXmlElement *red=color->FirstChildElement("red");
        TiXmlElement *green=color->FirstChildElement("green");
        TiXmlElement *blue=color->FirstChildElement("blue");
        if (red && red->GetText())
            bc.setColor(ToolBox::stringToInt(red->GetText()), ToolBox::stringToInt(green->GetText()), ToolBox::stringToInt(blue->GetText()));
        else
        {
            setStaticError(error, BadBcColor);
            return 0;
        }
    }
    else
    {
        setStaticError(error, BadBcColor);
        return 0;
    }

    return new BoundaryCondition(bc);
}

double BoundaryCondition::rh() const
{
    return _rh;
}

void BoundaryCondition::setRh(double rh, ErrorHandler *error)
{
    if (rh < 0.0 || rh > 1.0)
    {
        setError(error, BadBcRh);
        return;
    }

    _rh = rh;
    changed();
}
