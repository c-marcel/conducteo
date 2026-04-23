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

#include <Point2d.h>
#include <tools/ToolBox.h>
#include <math.h>

using namespace DxfReader;

Point2d::Point2d()
{
}

Point2d::~Point2d()
{
}

Entity::Type Point2d::getType() const
{
    return TypePoint;
}

void Point2d::setX(double x)
{
    _x=x;
}

void Point2d::setY(double y)
{
    _y=y;
}

double Point2d::x() const
{
    return _x;
}

double Point2d::y() const
{
    return _y;
}

bool Point2d::containsPoint(double x, double y, double epsilon) const
{
    double distance=sqrt((x-this->x())*(x-this->x())+(y-this->y())*(y-this->y()));
    return (distance<=epsilon);
}

double Point2d::minimalX() const
{
    return _x;
}

double Point2d::minimalY() const
{
    return _y;
}

double Point2d::maximalX() const
{
    return _x;
}

double Point2d::maximalY() const
{
    return _y;
}

std::string Point2d::toConducteoElement() const
{
    std::string xml="<point>";
    xml+="<x>"+ToolBox::doubleToString(_x)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_y)+"</y>";
    xml+="</point>";
    return xml;
}
