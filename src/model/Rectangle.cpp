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

#include <model/Rectangle.h>
#include <geometry/Line.h>

Rectangle::Rectangle()
{
}

Rectangle::~Rectangle()
{
}

void Rectangle::create(double x, double y, double width, double height)
{
    clean();

    Point p1;
    p1.setX(x);
    p1.setY(y);

    Point p2;
    p2.setX(x+width);
    p2.setY(y);

    Point p3;
    p3.setX(x+width);
    p3.setY(y+height);

    Point p4;
    p4.setX(x);
    p4.setY(y+height);

    Line *path1=new Line;
    path1->setBegin(p1, 0);
    path1->setEnd(p2, 0);
    push_back(path1);

    Line *path2=new Line;
    path2->setBegin(p2, 0);
    path2->setEnd(p3, 0);
    push_back(path2);

    Line *path3=new Line;
    path3->setBegin(p3, 0);
    path3->setEnd(p4, 0);
    push_back(path3);

    Line *path4=new Line;
    path4->setBegin(p4, 0);
    path4->setEnd(p1, 0);
    push_back(path4);

    computeBounds();
}

void Rectangle::setWidth(double width)
{
    Point p1=((Line*)at(0))->begin();
    Point p2=((Line*)at(0))->end();
    Point p3=((Line*)at(1))->end();

    p2.setX(p1.x()+width);
    p3.setX(p1.x()+width);

    ((Line*)at(0))->setEnd(p2, 0);
    ((Line*)at(1))->setBegin(p2, 0);
    ((Line*)at(1))->setEnd(p3, 0);
    ((Line*)at(2))->setBegin(p3, 0);

    computeBounds();
}

void Rectangle::setHeight(double height)
{
    Point p1=((Line*)at(0))->begin();
    Point p3=((Line*)at(1))->end();
    Point p4=((Line*)at(2))->end();

    p3.setY(p1.y()+height);
    p4.setY(p1.y()+height);

    ((Line*)at(1))->setEnd(p3, 0);
    ((Line*)at(2))->setBegin(p3, 0);
    ((Line*)at(2))->setEnd(p4, 0);
    ((Line*)at(3))->setBegin(p4, 0);

    computeBounds();
}

double Rectangle::x() const
{
    return ((Line*)at(0))->begin().x();
}

double Rectangle::y() const
{
    return ((Line*)at(0))->begin().y();
}

double Rectangle::width() const
{
    return ((Line*)at(0))->end().x()-((Line*)at(0))->begin().x();
}

double Rectangle::height() const
{
    return ((Line*)at(1))->end().y()-((Line*)at(1))->begin().y();
}
