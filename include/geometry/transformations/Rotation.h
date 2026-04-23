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

#ifndef ROTATION_H
#define ROTATION_H

#include <geometry/transformations/Transformation.h>
#include <string>

class Rotation: public Transformation
{

public:

    Rotation();
    virtual ~Rotation();

    virtual Point apply(const Point &point) const;
    virtual Point applyInverse(const Point &point) const;

    void setCenter(const Point &center);
    void setAngle(double angle);

    Point center() const;
    double angle() const;

    std::string serialize() const;
    static Rotation createFromXml(const std::string &xml, ErrorHandler *error);

private:

    Point   _center;
    double  _angle;

};

#endif
