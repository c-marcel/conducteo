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

#include <Entity.h>
#include <Layer.h>
#include <Block.h>
#include <algorithm>

using namespace DxfReader;

Entity::Entity():
    _redColor(0),
    _greenColor(0),
    _blueColor(0),
    _layer(0),
    _block(0),
    _width(0.0),
    _lineType(Continuous),
    _widthType(DefaultWidth),
    _visible(true),
    _selected(false),
    _zOrder(0)
{
}

Entity::~Entity()
{
}

void Entity::setLayer(Layer *layer)
{
    _layer=layer;
}

void Entity::setBlock(Block *block)
{
    _block=block;
}

void Entity::setZOrder(int zorder)
{
    _zOrder = zorder;
}

int Entity::zOrder() const
{
    return _zOrder;
}

void Entity::setColor(int red, int green, int blue)
{
    _redColor=red;
    _greenColor=green;
    _blueColor=blue;
}

void Entity::setWidth(WidthType type, double width)
{
    _widthType=type;
    _width=width;
}

Layer *Entity::layer() const
{
    return _layer;
}

Block *Entity::block() const
{
    return _block;
}

int Entity::redColor() const
{
    return _redColor;
}

int Entity::greenColor() const
{
    return _greenColor;
}

int Entity::blueColor() const
{
    return _blueColor;
}

double Entity::width() const
{
    if (_widthType==ByLayerWidth && _layer)
        return _layer->defaultWidth();

    if (_widthType==ByBlockWidth && _block)
        return _block->width();

    if (_widthType==CustomWidth)
        return _width;

    return 0.0;
}

Entity::Type Entity::getType() const
{
    return TypeVirtual;
} 

bool Entity::containsPoint(double x, double y, double epsilon) const
{
    return false;
}

double Entity::minimalX() const
{
    return 0.0;
}

double Entity::minimalY() const
{
    return 0.0;
}

double Entity::maximalX() const
{
    return 0.0;
}

double Entity::maximalY() const
{
    return 0.0;
}

void Entity::setLineType(LineType type)
{
    _lineType=type;
}

Entity::LineType Entity::lineType() const
{
    if (_lineType==ByLayerLineType && _layer)
        return _layer->defaultLineType();

    if (_widthType==ByBlockWidth && _block)
        return _block->lineType();

    return _lineType;
}

Entity::LineType Entity::lineTypeFromString(const std::string &type)
{
    std::string t=type;
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);

    if (t=="bylayer")
        return ByLayerLineType;
    if (t=="byblock")
        return ByBlockLineType;
    if (t=="continuous")
        return Continuous;
    if (t=="dot")
        return Dot;
    if (t=="dot2")
        return Dot2;
    if (t=="dotx2")
        return DotX2;
    if (t=="center")
        return Center;
    if (t=="centerx2")
        return CenterX2;
    if (t=="center2")
        return Center2;
    if (t=="dashed")
        return Dashed;
    if (t=="dashedx2")
        return DashedX2;
    if (t=="dashed2")
        return Dashed2;
    if (t=="phantom")
        return Phantom;
    if (t=="phantomx2")
        return PhantomX2;
    if (t=="phantom2")
        return Phantom2;
    if (t=="dashdot")
        return DashDot;
    if (t=="dashdotx2")
        return DashDotX2;
    if (t=="dashdot2")
        return DashDot2;
    if (t=="divide")
        return Divide;
    if (t=="dividex2")
        return DivideX2;
    if (t=="divide2")
        return Divide2;
    return Continuous;
}

bool Entity::visible() const
{
    return _visible;
}

void Entity::setVisible(bool visible)
{
    _visible=visible;
}

void Entity::setSelected(bool selected)
{
    _selected=selected;
}

bool Entity::selected() const
{
    return _selected;
}

std::string Entity::toConducteoElement() const
{
    // Do nothing.
    return "";
}
