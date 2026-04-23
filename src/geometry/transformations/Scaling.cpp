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

#include <geometry/transformations/Scaling.h>

Scaling::Scaling():
    _type(NoScaling),
    _xFactor(1.0),
    _yFactor(1.0)
{
}

Scaling::~Scaling()
{
}

Point Scaling::apply(const Point &point) const
{
    Point p;
    
    double x=point.x();
    if (_type & XScaling)
        x=x*_xFactor;
        
    double y=point.y();
    if (_type & YScaling)
        y=y*_yFactor;
        
    p.setX(x);
    p.setY(y);
    
    return p;
}

Point Scaling::applyInverse(const Point &point) const
{
    Scaling scaling=*this;
    scaling.setXScalingFactor(1.0/scaling.xScalingFactor());
    scaling.setYScalingFactor(1.0/scaling.yScalingFactor());
    return scaling.apply(point);
}

void Scaling::setType(Type type)
{
    _type=type;
}

void Scaling::setXScalingFactor(double factor)
{
    _xFactor=factor;
}

void Scaling::setYScalingFactor(double factor)
{
    _yFactor=factor;
}

Scaling::Type Scaling::type() const
{
    return _type;
}

double Scaling::xScalingFactor() const
{
    return _xFactor;
}

double Scaling::yScalingFactor() const
{
    return _yFactor;
}
