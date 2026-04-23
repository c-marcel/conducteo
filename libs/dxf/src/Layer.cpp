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

#include <Layer.h>
#include <Entity.h>

using namespace DxfReader;

Layer::Layer(): 
    _visible(true),
    _redColor(255),
    _greenColor(255),
    _blueColor(255),
    _defaultWidth(0.0),
    _defaultLineType(Entity::Continuous)
{
}

Layer::~Layer()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
}

void Layer::setName(const std::string &name)
{
    _name=name;
}

std::string Layer::getName() const
{
    return _name;
}

void Layer::setVisible(bool visible)
{
    _visible=visible;
}

bool Layer::isVisible() const
{
    return _visible;
}

void Layer::setColor(int red, int green, int blue)
{
    _redColor=red;
    _greenColor=green;
    _blueColor=blue;
}

int Layer::redColor() const
{
    return _redColor;
}

int Layer::greenColor() const
{
    return _greenColor;
}

int Layer::blueColor() const
{
    return _blueColor;
}

double Layer::minimalX() const
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

double Layer::minimalY() const
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

double Layer::maximalX() const
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

double Layer::maximalY() const
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

void Layer::removeEntityReference(Entity *entity)
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)==entity)
        {
            erase(begin()+i);
            break;
        }
    }
}

void Layer::setDefaultWidth(double width)
{
    _defaultWidth=width;
}

double Layer::defaultWidth() const
{
    return _defaultWidth;
}

void Layer::setDefaultLineType(Entity::LineType type)
{
    _defaultLineType=type;
}

Entity::LineType Layer::defaultLineType() const
{
    return _defaultLineType;
}
