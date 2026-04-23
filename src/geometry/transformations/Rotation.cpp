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

#include <geometry/transformations/Rotation.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>

#define _USE_MATH_DEFINES
#include <math.h>

Rotation::Rotation():
    _angle(0.0)
{
}

Rotation::~Rotation()
{
}

void Rotation::setCenter(const Point &center)
{
    _center=center;
}

void Rotation::setAngle(double angle)
{
    _angle=angle;
}

Point Rotation::center() const
{
    return _center;
}

double Rotation::angle() const
{
    return _angle;
}

std::string Rotation::serialize() const
{
    std::string xml="<rotation>";
    xml+="<center>";
    xml+=_center.serialize();
    xml+="</center>";
    xml+="<angle>"+ToolBox::doubleToString(_angle)+"</angle>";
    xml+="</rotation>";
    return xml;
}

Rotation Rotation::createFromXml(const std::string &xml, ErrorHandler *error)
{
    Rotation r;

    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *rotation=doc.RootElement();
    if (!rotation)
    {
        setStaticError(error, BadRotationNode);
        return r;
    }

    // Center.
    TiXmlElement *center=rotation->FirstChildElement("center");
    if (center && center->FirstChildElement("point"))
    {
        std::string point_s=ToolBox::domToString(center->FirstChildElement("point"));
        r.setCenter(Point::createFromXml(point_s, error));
    }
    else
    {
        setStaticError(error, BadRotationCenter);
        return r;
    }

    // Angle.
    TiXmlElement *angle=rotation->FirstChildElement("angle");
    if (angle && angle->GetText())
        r.setAngle(ToolBox::convertStringToDouble(angle->GetText()));
    else
    {
        setStaticError(error, BadRotationAngle);
        return r;
    }

    return r;
}

Point Rotation::apply(const Point &point) const
{
    Point p;

    // Move rotation center to origin.
    double x=point.x()-_center.x();
    double y=point.y()-_center.y();

    // Rotate point.
    double theta=_angle/180.*M_PI;
    
    double xr=x*cos(theta)-y*sin(theta);
    double yr=x*sin(theta)+y*cos(theta);

    // Move origin to rotation center.
    xr+=_center.x();
    yr+=_center.y();

    p.setX(xr);
    p.setY(yr);

    return p;
}

Point Rotation::applyInverse(const Point &point) const
{
    Rotation inverse=*this;
    inverse.setAngle(-inverse.angle());
    return inverse.apply(point);
}
