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

#include <Vertex.h>
#include <tools/ToolBox.h>

using namespace DxfReader;

Vertex::Vertex():
    _bulge(0.0),
    _z(0.0)
{
}

Vertex::~Vertex()
{
}

void Vertex::setPosition(double x, double y)
{
    _x=x;
    _y=y;
}

void Vertex::setBulge(double bulge)
{
    _bulge=bulge;
}

double Vertex::x() const
{
    return _x;
}

double Vertex::y() const
{
    return _y;
}

double Vertex::bulge() const
{
    return _bulge;
}

void Vertex::setHeight(double height)
{
    _z=height;
}

double Vertex::height() const
{
    return _z;
}

std::string Vertex::toConducteoElement() const
{
    std::string xml="<point>";
    xml+="<x>"+ToolBox::doubleToString(_x)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_y)+"</y>";
    xml+="</point>";
    return xml;
}
