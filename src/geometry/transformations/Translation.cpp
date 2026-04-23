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

#include <geometry/transformations/Translation.h>

Translation::Translation():
    _horizontal(0.0),
    _vertical(0.0)
{
}

Translation::~Translation()
{
}

Point Translation::apply(const Point &point) const
{
    // Apply translation.
    double x=point.x()+_horizontal;
    double y=point.y()+_vertical;
    
    Point p;
    p.setX(x);
    p.setY(y);
    return p;
}

Point Translation::applyInverse(const Point &point) const
{
    Translation translation=*this;
    translation.setHorizontalDistance(-translation.horizontalDistance());
    translation.setVerticalDistance(-translation.verticalDistance());
    return translation.apply(point);
}

void Translation::setHorizontalDistance(double distance)
{
    _horizontal=distance;
}

void Translation::setVerticalDistance(double distance)
{
    _vertical=distance;
}

double Translation::horizontalDistance() const
{
    return _horizontal;
}

double Translation::verticalDistance() const
{
    return _vertical;
}
