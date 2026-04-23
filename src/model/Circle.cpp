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

#include <model/Circle.h>
#include <geometry/Arc.h>
#include <math.h>

using namespace std;

Circle::Circle()
{
}

Circle::~Circle()
{
}

void Circle::create(double x, double y, double radius)
{
    clean();

    Point c;
    c.setX(x);
    c.setY(y);

    Arc *arc=new Arc;
    arc->setCenter(c);
    arc->setHorizontalRadius(fabs(radius), 0);
    arc->setVerticalRadius(fabs(radius), 0);

    c.setX(x+radius);
    arc->setBegin(c, 0);
    arc->setEnd(c, 0);
    arc->recreateLines();
    push_back(arc);

    computeBounds();
}

void Circle::setRadius(double radius)
{
    ((Arc*)at(0))->setHorizontalRadius(fabs(radius), 0);
    ((Arc*)at(0))->setVerticalRadius(fabs(radius), 0);

    Point c;
    c.setX(x()+radius);
    c.setY(y());
    
    ((Arc*)at(0))->setBegin(c, 0);
    ((Arc*)at(0))->setEnd(c, 0);
    ((Arc*)at(0))->recreateLines();

    computeBounds();
}

double Circle::x() const
{
    return ((Arc*)at(0))->center().x();
}

double Circle::y() const
{
    return ((Arc*)at(0))->center().y();
}

double Circle::radius() const
{
    return ((Arc*)at(0))->horizontalRadius();
}
