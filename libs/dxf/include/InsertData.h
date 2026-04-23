// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2019, Clément MARCEL.
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

#ifndef _INSERT_DATA_H
#define _INSERT_DATA_H

#include <Entity.h>
#include <string>

namespace DxfReader
{

class Block;

class InsertData: public Entity
{

public:

    InsertData();
    virtual ~InsertData();
    virtual Type getType() const;
    virtual bool containsPoint(double x, double y, double epsilon) const;
    virtual double minimalX() const;
    virtual double minimalY() const;
    virtual double maximalX() const;
    virtual double maximalY() const;

    void setAngle(double angle);
    void setInsertionPoint(double x, double y);
    void setScaleFactors(double sx, double sy);

    double angle() const;
    double insertionPointX() const;
    double insertionPointY() const;
    double scaleX() const;
    double scaleY() const;

    std::string getBlockName() const;
    void setBlockName(const std::string &blockName);

private:

    double transformPointX(double x, double y) const;
    double transformPointY(double x, double y) const;

    double      _x;
    double      _y;
    double      _sx;
    double      _sy;
    double      _angle;
    std::string _blockName;

};

}

#endif
