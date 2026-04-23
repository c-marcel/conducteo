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

#include <Circle.h>
#include <tools/ToolBox.h>
#include <algorithm>
#include <math.h>

using namespace DxfReader;
using namespace std;

Circle::Circle():
    _radius(0.0),
    _xCenter(0.0),
    _yCenter(0.0),
    _filled(false)
{
}

Circle::~Circle()
{
}

Entity::Type Circle::getType() const
{
    return TypeCircle;
}

bool Circle::containsPoint(double x, double y, double epsilon) const
{
    x=x-_xCenter;
    y=y-_yCenter;

    double distance=sqrt(x*x+y*y);
    if (fabs(distance-_radius)>epsilon)
        return false;
    return true;
}

double Circle::minimalX() const
{
    return _xCenter-_radius;
}

double Circle::minimalY() const
{
    return _yCenter-_radius;
}

double Circle::maximalX() const
{
    return _xCenter+_radius;
}

double Circle::maximalY() const
{
    return _yCenter+_radius;
}

void Circle::setCenter(double x, double y)
{
    _xCenter=x;
    _yCenter=y;
}

void Circle::setRadius(double radius)
{
    _radius = radius;
}

double Circle::xCenter() const
{
    return _xCenter;
}

double Circle::yCenter() const
{
    return _yCenter;
}

double Circle::radius() const
{
    return _radius;
}

void Circle::setFilled(bool filled)
{
    _filled=filled;
}

bool Circle::filled() const
{
    return _filled;
}

std::string Circle::toConducteoElement() const
{
    std::string xml="<arc>";
    
    xml+="<center>";
    xml+="<point>";
    xml+="<x>"+ToolBox::doubleToString(_xCenter)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_yCenter)+"</y>";
    xml+="</point>";
    xml+="</center>";

    xml+="<horizontal-radius>"+ToolBox::doubleToString(_radius)+"</horizontal-radius>";
    xml+="<vertical-radius>"+ToolBox::doubleToString(_radius)+"</vertical-radius>";
    
    xml+="<begin-point>";
    xml+="<point>";
    xml+="<x>"+ToolBox::doubleToString(_xCenter+_radius)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_yCenter)+"</y>";
    xml+="</point>";
    xml+="</begin-point>";

    xml+="<end-point>";
    xml+="<point>";
    xml+="<x>"+ToolBox::doubleToString(_xCenter+_radius)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_yCenter)+"</y>";
    xml+="</point>";
    xml+="</end-point>";

    xml+="<rotation-angle>0.0</rotation-angle>";

    xml+="</arc>";
    return xml;
}
