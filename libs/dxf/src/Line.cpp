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

#include <Line.h>
#include <algorithm>
#include <cmath>

using namespace DxfReader;

Line::Line()
{
}

Line::~Line()
{
}

Entity::Type Line::getType() const
{
    return TypeLine;
} 

bool Line::containsPoint(double x, double y, double epsilon) const
{
    epsilon+=width();

    double dx=_end.x()-_begin.x();
    double dy=_end.y()-_begin.y();
    double length=sqrt(dx*dx+dy*dy);
    if (length<1.0e-6)
        return false;

    double p1m_x=x-_begin.x();
    double p1m_y=y-_begin.y();
    double p1m_length=sqrt(p1m_x*p1m_x+p1m_y*p1m_y);
    p1m_x=p1m_x/p1m_length;
    p1m_y=p1m_y/p1m_length;

    double p1p2_x=_end.x()-_begin.x();
    double p1p2_y=_end.y()-_begin.y();
    double p1p2_length=sqrt(p1p2_x*p1p2_x+p1p2_y*p1p2_y);
    p1p2_x=p1p2_x/p1p2_length;
    p1p2_y=p1p2_y/p1p2_length;

    double v=p1m_x*p1p2_y-p1m_y*p1p2_x;
    if (std::fabs(v)>epsilon/p1p2_length)
        return false;

    p1m_x=x-_begin.x();
    p1m_y=y-_begin.y();

    p1p2_x=_end.x()-_begin.x();
    p1p2_y=_end.y()-_begin.y();

    double s=p1m_x*p1p2_x+p1m_y*p1p2_y;
    if (s<0.0)
        return false;
    if (s>p1p2_length*p1p2_length)
        return false;
    return true;
}

Point2d Line::begin() const
{
    return _begin;
}

Point2d Line::end() const
{
    return _end;
}

void Line::setBegin(const Point2d &begin)
{
    _begin=begin;
}

void Line::setEnd(const Point2d &end)
{
    _end=end;
}

double Line::minimalX() const
{
    return std::min(_begin.minimalX(), _end.minimalX());
}

double Line::minimalY() const
{
    return std::min(_begin.minimalY(), _end.minimalY());
}

double Line::maximalX() const
{
    return std::max(_begin.maximalX(), _end.maximalX());
}

double Line::maximalY() const
{
    return std::max(_begin.maximalY(), _end.maximalY());
}

std::string Line::toConducteoElement() const
{
    std::string xml="<line>";
    xml+="<begin>";
    xml+=_begin.toConducteoElement();
    xml+="</begin>";

    xml+="<end>";
    xml+=_end.toConducteoElement();
    xml+="</end>";
    xml+="</line>";
    return xml;
}
