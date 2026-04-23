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

#include <model/Image.h>
#include <errors/ErrorHandler.h>
#include <model/ImageData.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>

Image::Image():
    _x(0.0),
    _y(0.0),
    _width(0.0),
    _height(0.0),
    _imageData(0),
    _selected(false),
    _id(ToolBox::getNewUuuid())
{
}

Image::Image(const Image &image)
{
    _x=image._x;
    _y=image._y;
    _width=image._width;
    _height=image._height;
    _data=image._data;
    _selected=image._selected;
    _imageData=0;
    _id=image._id;
}

void Image::setId(const std::string &id)
{
    _id=id;
}

std::string Image::id() const
{
    return _id;
}

Image::~Image()
{
    if (_imageData)
        delete _imageData;
}

void Image::setX(double x)
{
    _x=x;
}

void Image::setY(double y)
{
    _y=y;
}

void Image::setWidth(double width)
{
    _width=width;
}

void Image::setHeight(double height)
{
    _height=height;
}

void Image::setData(const std::string &data)
{
    _data=data;
}

double Image::x() const
{
    return _x;
}

double Image::y() const
{
    return _y;
}

double Image::width() const
{
    return _width;
}

double Image::height() const
{
    return _height;
}

std::string Image::data() const
{
    return _data;
}

Image *Image::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *image=doc.RootElement();
    if (!image)
    {
        setStaticError(error, BadImageNode);
        return 0;
    }

    if (!image->Value())
    {
        setStaticError(error, BadImageNode);
        return 0;
    }

    std::string root_name=image->Value();
    if (root_name!="image")
    {
        setStaticError(error, BadImageNode);
        return 0;
    }

    Image *im=new Image;

    // X.
    TiXmlElement *x=image->FirstChildElement("x");
    if (x && x->GetText())
        im->setX(ToolBox::stringToDouble(x->GetText()));

    // Y.
    TiXmlElement *y=image->FirstChildElement("y");
    if (y && y->GetText())
        im->setY(ToolBox::stringToDouble(y->GetText()));

    // Width.
    TiXmlElement *width=image->FirstChildElement("width");
    if (width && width->GetText())
        im->setWidth(ToolBox::stringToDouble(width->GetText()));

    // Height.
    TiXmlElement *height=image->FirstChildElement("height");
    if (height && height->GetText())
        im->setHeight(ToolBox::stringToDouble(height->GetText()));

    // Data.
    TiXmlElement *data=image->FirstChildElement("data");
    if (data && data->GetText())
        im->setData(data->GetText());

    return im;
}

std::string Image::serialize() const
{
    std::string xml;
    xml+="<image>";
    xml+="<x>"+ToolBox::doubleToString(_x)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_y)+"</y>";
    xml+="<width>"+ToolBox::doubleToString(_width)+"</width>";
    xml+="<height>"+ToolBox::doubleToString(_height)+"</height>";
    xml+="<data>"+_data+"</data>";
    xml+="</image>";
    return xml;
}

void Image::setImageData(ImageData *data)
{
    _imageData=data;
}

ImageData *Image::imageData() const
{
    return _imageData;
}

void Image::setSelected(bool selected)
{
    _selected=selected;
}

bool Image::selected() const
{
    return _selected;
}
