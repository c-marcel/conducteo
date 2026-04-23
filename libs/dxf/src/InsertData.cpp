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

#include <InsertData.h>
#include <Block.h>

using namespace DxfReader;

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

InsertData::InsertData():
    _x(0.0),
    _y(0.0),
    _sx(1.0),
    _sy(1.0),
    _angle(0.0)
{
}

InsertData::~InsertData()
{
}

Entity::Type InsertData::getType() const
{
    return TypeInsertData;
} 

bool InsertData::containsPoint(double x, double y, double epsilon) const
{
    Block *block=this->block();
    if (!block)
        return false;

    double alpha=angle()/180.*M_PI;

    x -= insertionPointX();
    y -= insertionPointY();

    x += block->x();
    y += block->y();

    double x2=x*cos(alpha)+y*sin(alpha);
    double y2=y*cos(alpha)-x*sin(alpha);

    x2=x2/scaleX();
    y2=y2/scaleY();

    return block->containsPoint(x2, y2, epsilon);
}

double InsertData::minimalX() const
{
    Block *block=this->block();
    if (!block)
        return 0.0;

    double minimalX=block->minimalX();
    double minimalY=block->minimalY();
    double maximalX=block->maximalX();
    double maximalY=block->maximalY();

    double x1=transformPointX(minimalX, minimalY);
    double y1=transformPointY(minimalX, minimalY);

    double x2=transformPointX(maximalX, maximalY);
    double y2=transformPointY(maximalX, maximalY);

    double x3=transformPointX(minimalX, maximalY);
    double y3=transformPointY(minimalX, maximalY);

    double x4=transformPointX(maximalX, minimalY);
    double y4=transformPointY(maximalX, minimalY);

    double xmin=std::min(x1, x2);
    double xmin2=std::min(x3, x4);
    return std::min(xmin, xmin2);
}

double InsertData::minimalY() const
{
    Block *block=this->block();
    if (!block)
        return 0.0;

    double minimalX=block->minimalX();
    double minimalY=block->minimalY();
    double maximalX=block->maximalX();
    double maximalY=block->maximalY();

    double x1=transformPointX(minimalX, minimalY);
    double y1=transformPointY(minimalX, minimalY);

    double x2=transformPointX(maximalX, maximalY);
    double y2=transformPointY(maximalX, maximalY);

    double x3=transformPointX(minimalX, maximalY);
    double y3=transformPointY(minimalX, maximalY);

    double x4=transformPointX(maximalX, minimalY);
    double y4=transformPointY(maximalX, minimalY);

    double ymin=std::min(y1, y2);
    double ymin2=std::min(y3, y4);
    return std::min(ymin, ymin2);
}

double InsertData::maximalX() const
{
    Block *block=this->block();
    if (!block)
        return 0.0;

    double minimalX=block->minimalX();
    double minimalY=block->minimalY();
    double maximalX=block->maximalX();
    double maximalY=block->maximalY();

    double x1=transformPointX(minimalX, minimalY);
    double y1=transformPointY(minimalX, minimalY);

    double x2=transformPointX(maximalX, maximalY);
    double y2=transformPointY(maximalX, maximalY);

    double x3=transformPointX(minimalX, maximalY);
    double y3=transformPointY(minimalX, maximalY);

    double x4=transformPointX(maximalX, minimalY);
    double y4=transformPointY(maximalX, minimalY);

    double xmin=std::max(x1, x2);
    double xmin2=std::max(x3, x4);
    return std::max(xmin, xmin2);
}

double InsertData::maximalY() const
{
    Block *block=this->block();
    if (!block)
        return 0.0;

    double minimalX=block->minimalX();
    double minimalY=block->minimalY();
    double maximalX=block->maximalX();
    double maximalY=block->maximalY();

    double x1=transformPointX(minimalX, minimalY);
    double y1=transformPointY(minimalX, minimalY);

    double x2=transformPointX(maximalX, maximalY);
    double y2=transformPointY(maximalX, maximalY);

    double x3=transformPointX(minimalX, maximalY);
    double y3=transformPointY(minimalX, maximalY);

    double x4=transformPointX(maximalX, minimalY);
    double y4=transformPointY(maximalX, minimalY);

    double ymin=std::max(y1, y2);
    double ymin2=std::max(y3, y4);
    return std::max(ymin, ymin2);
}

double InsertData::transformPointX(double x, double y) const
{
    double alpha=angle()/360.*M_PI;
    x=x*scaleX();
    x=x*cos(alpha)-y*sin(alpha);
    x+=insertionPointX();
    return x;
}

double InsertData::transformPointY(double x, double y) const
{
    double alpha=angle()/360.*M_PI;
    y=y*scaleY();
    y=y*cos(alpha)+x*sin(alpha);
    y+=insertionPointY();
    return y;
}

std::string InsertData::getBlockName() const
{
    return _blockName;
}

void InsertData::setBlockName(const std::string &blockName)
{
    _blockName = blockName;
}

void InsertData::setAngle(double angle)
{
    _angle=angle;
}

void InsertData::setInsertionPoint(double x, double y)
{
    _x=x;
    _y=y;
}

void InsertData::setScaleFactors(double sx, double sy)
{
    _sx=sx;
    _sy=sy;
}

double InsertData::insertionPointX() const
{
    return _x;
}

double InsertData::insertionPointY() const
{
    return _y;
}

double InsertData::angle() const
{
    return _angle;
}

double InsertData::scaleX() const
{
    return _sx;
}

double InsertData::scaleY() const
{
    return _sy;
}
