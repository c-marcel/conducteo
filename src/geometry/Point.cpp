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

#include <geometry/Point.h>
#include <geometry/GeometryTools.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>

Point::Point():
    _x(0.0),
    _y(0.0)
{
}

Point::~Point()
{
}

void Point::setX(double x)
{
    _x=x;
}

void Point::setY(double y)
{
    _y=y;
}

double Point::x() const
{
    return _x;
}

double Point::y() const
{
    return _y;
}

std::string Point::serialize() const
{
    std::string xml="<point>";
    xml+="<x>"+ToolBox::doubleToString(_x)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_y)+"</y>";
    xml+="</point>";
    return xml;
}

Point Point::createFromXml(const std::string &xml, ErrorHandler *error)
{
    Point p;

    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *point=doc.RootElement();
    if (!point)
    {
        setStaticError(error, BadPointNode);
        return p;
    }

    TiXmlElement *x=point->FirstChildElement("x");
    if (x && x->GetText())
        p.setX(ToolBox::convertStringToDouble(x->GetText()));
    else
    {
        setStaticError(error, BadPointXValue);
        return p;
    }

    TiXmlElement *y=point->FirstChildElement("y");
    if (y && y->GetText())
        p.setY(ToolBox::convertStringToDouble(y->GetText()));
    else
    {
        setStaticError(error, BadPointYValue);
        return p;
    }

    return p;
}
