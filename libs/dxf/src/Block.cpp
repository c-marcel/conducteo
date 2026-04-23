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

#include <Block.h>
#include <InsertData.h>
#include <Entity.h>

using namespace DxfReader;

Block::Block(): 
    _x(0.0),
    _y(0.0)
{
}

Block::~Block()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
}

Entity::Type Block::getType() const
{
    return TypeBlock;
}

bool Block::containsPoint(double x, double y, double epsilon) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Entity *entity=at(i);
        if (entity->containsPoint(x, y, epsilon))
            return true;
    }
    return false;
}

void Block::setName(const std::string &name)
{
    _name=name;
}

std::string Block::name() const
{
    return _name;
}

double Block::minimalX() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Entity *entity=at(i);
        if (entity->minimalX()<v || !defined)
        {
            v=entity->minimalX();
            defined=true;
        }
    }
    return v;
}

double Block::minimalY() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Entity *entity=at(i);
        if (entity->minimalY()<v || !defined)
        {
            v=entity->minimalY();
            defined=true;
        }
    }
    return v;
}

double Block::maximalX() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Entity *entity=at(i);
        if (entity->maximalX()>v || !defined)
        {
            v=entity->maximalX();
            defined=true;
        }
    }
    return v;
}

double Block::maximalY() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Entity *entity=at(i);
        if (entity->maximalY()>v || !defined)
        {
            v=entity->maximalY();
            defined=true;
        }
    }
    return v;
}

void Block::setX(double x)
{
    _x=x;
}

void Block::setY(double y)
{
    _y=y;
}

double Block::x() const
{
    return _x;
}

double Block::y() const
{
    return _y;
}
