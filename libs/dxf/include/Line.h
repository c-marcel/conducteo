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

#ifndef _LINE_H
#define _LINE_H

#include <Entity.h>
#include <Point2d.h>

namespace DxfReader
{

class Line: public Entity
{

public:

    Line();
    virtual ~Line();
    virtual Type getType() const;
    virtual bool containsPoint(double x, double y, double epsilon) const;
    virtual double minimalX() const;
    virtual double minimalY() const;
    virtual double maximalX() const;
    virtual double maximalY() const;

    void setBegin(const Point2d &begin);
    void setEnd(const Point2d &end);

    Point2d begin() const;
    Point2d end() const;

    virtual std::string toConducteoElement() const;

private:

    Point2d      _begin;
    Point2d      _end;

};

}

#endif
