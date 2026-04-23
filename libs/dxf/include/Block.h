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

#ifndef _BLOCK_H
#define _BLOCK_H

#include <Entity.h>
#include <vector>

namespace DxfReader
{

class Block: public std::vector<Entity*>,
             public Entity
{

public:

    Block();
    virtual ~Block();
    virtual Type getType() const;
    virtual bool containsPoint(double x, double y, double epsilon) const;
    virtual double minimalX() const;
    virtual double minimalY() const;
    virtual double maximalX() const;
    virtual double maximalY() const;

    void setName(const std::string &name);
    std::string name() const;

    void setX(double x);
    void setY(double y);
    double x() const;
    double y() const;

private:

    std::string     _name;
    double          _x;
    double          _y;

};

}

#endif
