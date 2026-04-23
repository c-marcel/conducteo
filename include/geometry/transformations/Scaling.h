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

#ifndef SCALING_H
#define SCALING_H

#include <geometry/transformations/Transformation.h>

class Scaling: public Transformation
{

public:

    enum Type
    {
        NoScaling=0,
        XScaling =1,
        YScaling =2
    };

    Scaling();
    virtual ~Scaling();

    virtual Point apply(const Point &point) const;
    virtual Point applyInverse(const Point &point) const;
    
    void setType(Type type);
    void setXScalingFactor(double factor);
    void setYScalingFactor(double factor);

    Type type() const;
    double xScalingFactor() const;
    double yScalingFactor() const;

private:

    Type    _type;
    double  _xFactor;
    double  _yFactor;

};

#endif
