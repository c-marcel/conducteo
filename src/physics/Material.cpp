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

#include <physics/Material.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>
#include <math.h>

Material::Material():
    _conductivity(1.0),
    _red(0),
    _green(0),
    _blue(0),
    _id(ToolBox::getNewUuuid()),
    _emissivity(0.9),
    _favorite(false),
    _mu(1.0)
{
}

Material::Material(const Material &material)
{
    _name=material._name;
    _description=material._description;
    _id=material._id;
    _category=material._category;
    _conductivity=material._conductivity;
    _mu = material._mu;
    _emissivity=material._emissivity;
    _red=material._red;
    _green=material._green;
    _blue=material._blue;
    _favorite=material._favorite;
}

Material::~Material()
{
}

bool Material::check(ErrorHandler *error)
{
    if (_conductivity<=0.0)
    {
        setError(error, BadConductivity);
        return false;
    }

    if (_name.empty())
    {
        setError(error, BadMaterialName);
        return false;
    }

    if (_id.empty())
    {
        setError(error, BadMaterialId);
        return false;
    }

    if (_red>255)
    {
        setError(error, BadMaterialColor);
        return false;
    }

    if (_green>255)
    {
        setError(error, BadMaterialColor);
        return false;
    }

    if (_blue>255)
    {
        setError(error, BadMaterialColor);
        return false;
    }

    if (_emissivity>1.0 || _emissivity<0.0)
    {
        setError(error, BadMaterialEmissivity);
        return false;
    }

    return true;
}

void Material::setName(const std::string &name)
{
    _name=name;
    changed();
}

void Material::setDescription(const std::string &description)
{
    _description=description;
    changed();
}

void Material::setId(const std::string &id)
{
    _id=id;
    changed();
}

void Material::setConductivity(double conductivity, ErrorHandler *error)
{
    if (conductivity<=0.0)
    {
        setError(error, BadConductivity);
        return;
    }
    _conductivity=conductivity;
    changed();
}

void Material::setColor(unsigned int red, unsigned int green, unsigned int blue)
{
    _red=red;
    _green=green;
    _blue=blue;
    changed();
}

std::string Material::name() const
{
    return _name;
}

std::string Material::description() const
{
    return _description;
}

std::string Material::id() const
{
    return _id;
}

double Material::conductivity() const
{
    return _conductivity;
}

double Material::mu() const
{
    return _mu;
}

unsigned int Material::red() const
{
    return _red;
}

unsigned int Material::green() const
{
    return _green;
}

unsigned int Material::blue() const
{
    return _blue;
}

std::string Material::serialize() const
{
    std::string xml="<material id=\""+id()+"\" >";
    xml+="<name>"+ToolBox::escapeXmlCharacters(_name)+"</name>";
    xml+="<description>"+ToolBox::escapeXmlCharacters(_description)+"</description>";
    xml+="<conductivity>"+ToolBox::doubleToString(_conductivity)+"</conductivity>";
    xml+="<mu>"+ToolBox::doubleToString(_mu)+"</mu>";
    xml+="<emissivity>"+ToolBox::doubleToString(_emissivity)+"</emissivity>";
    std::string f="false";
    if (favorite())
        f="true";
    xml+="<favorite>"+f+"</favorite>";

    if (!_category.empty())
        xml+="<category>"+_category+"</category>";

    xml+="<color>";
    xml+="<red>"+ToolBox::intToString(_red)+"</red>";
    xml+="<green>"+ToolBox::intToString(_green)+"</green>";
    xml+="<blue>"+ToolBox::intToString(_blue)+"</blue>";
    xml+="</color>";
    xml+="</material>";
    return xml;
}

bool Material::isEqualTo(const Material *material) const
{
    if (!isEquivalentTo(material))
        return false;
    if (_id!=material->id())
        return false;
    if (_description!=material->description())
        return false;

    return true;
}

bool Material::isEquivalentTo(const Material *material) const
{
    if (_name!=material->name())
        return false;

    if (fabs(_conductivity-material->conductivity())>1.0e-5)
        return false;

    if (fabs(_mu - material->mu()) > 1.0e-5)
        return false;

    if (fabs(_emissivity-material->emissivity())>1.0e-5)
        return false;

    if (_red!=material->red())
        return false;
    if (_green!=material->green())
        return false;
    if (_blue!=material->blue())
        return false;

    return true;
}

Material *Material::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *material=doc.RootElement();
    if (!material)
    {
        setStaticError(error, BadMaterialNode);
        return 0;
    }

    if (!material->Value())
    {
        setStaticError(error, BadMaterialNode);
        return 0;
    }

    std::string root_name=material->Value();
    if (root_name!="material")
    {
        setStaticError(error, BadMaterialNode);
        return 0;
    }

    Material m;

    if (material->Attribute("id"))
        m.setId(material->Attribute("id"));
    else
    {
        setStaticError(error, BadMaterialId);
        return 0;
    }

    TiXmlElement *name=material->FirstChildElement("name");
    if (name && name->GetText())
        m.setName(name->GetText());
    else
    {
        setStaticError(error, BadMaterialName);
        return 0;
    }

    TiXmlElement *description=material->FirstChildElement("description");
    if (description && description->GetText())
        m.setDescription(description->GetText());

    TiXmlElement *conductivity=material->FirstChildElement("conductivity");
    if (conductivity && conductivity->GetText())
        m.setConductivity(ToolBox::convertStringToDouble(conductivity->GetText()), error);
    else
    {
        setStaticError(error, BadConductivity);
        return 0;
    }

    TiXmlElement *mu = material->FirstChildElement("mu");
    if (mu && mu->GetText())
        m.setMu(ToolBox::convertStringToDouble(mu->GetText()), error);
    else
        m.setMu(1.0, error);

    TiXmlElement *emissivity=material->FirstChildElement("emissivity");
    if (emissivity && emissivity->GetText())
        m.setEmissivity(ToolBox::convertStringToDouble(emissivity->GetText()), error);
    else
        m.setEmissivity(0.9, error);

    TiXmlElement *favorite=material->FirstChildElement("favorite");
    if (favorite && favorite->GetText() && std::string(favorite->GetText())=="true")
        m.setFavorite(true);

    TiXmlElement *category=material->FirstChildElement("category");
    if (category && category->GetText())
        m.setCategory(category->GetText());
    
    TiXmlElement *color=material->FirstChildElement("color");
    if (color)
    {
        TiXmlElement *red=color->FirstChildElement("red");
        TiXmlElement *green=color->FirstChildElement("green");
        TiXmlElement *blue=color->FirstChildElement("blue");
        if (red && red->GetText())
            m.setColor(ToolBox::stringToInt(red->GetText()), ToolBox::stringToInt(green->GetText()), ToolBox::stringToInt(blue->GetText()));
        else
        {
            setStaticError(error, BadMaterialColor);
            return 0;
        }
    }
    else
    {
        setStaticError(error, BadMaterialColor);
        return 0;
    }

    return new Material(m);
}

void Material::setEmissivity(double emissivity, ErrorHandler *error)
{
    if (emissivity<0.0 || emissivity>1.0)
    {
        setError(error, BadMaterialEmissivity);
        return;
    }
    _emissivity=emissivity;
    changed();
}

void Material::setMu(double mu, ErrorHandler *error)
{
    if (mu<0.0)
    {
        setError(error, BadMaterialMu);
        return;
    }

    _mu = mu;
    changed();
}

double Material::emissivity() const
{
    return _emissivity;
}

void Material::setFavorite(bool favorite)
{
    _favorite=favorite;
    changed();
}

bool Material::favorite() const
{
    return _favorite;
}

void Material::setCategory(const std::string &category)
{
    _category=category;
}

std::string Material::category() const
{
    return _category;
}
