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

#ifndef _VERTEX_H
#define _VERTEX_H

#include <string>

namespace DxfReader
{

class Vertex
{

public:

    Vertex();
    virtual ~Vertex();

    void setPosition(double x, double y);
    void setBulge(double bulge);
    void setHeight(double height);

    double x() const;
    double y() const;
    double bulge() const;
    double height() const;

    virtual std::string toConducteoElement() const;

private:

    double      _x;
    double      _y;
    double      _z;
    double      _bulge;

};

}

#endif
