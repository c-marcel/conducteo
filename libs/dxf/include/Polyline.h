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

#ifndef _POLYLINE_H
#define _POLYLINE_H

#include <Point2d.h>
#include <Entity.h>
#include <Vertex.h>
#include <vector>
#include <Line.h>

namespace DxfReader
{

class Polyline: public Entity,
                public std::vector<Vertex>
{

public:

    Polyline();
    virtual ~Polyline();
    virtual Type getType() const;
    virtual bool containsPoint(double x, double y, double epsilon) const;
    virtual double minimalX() const;
    virtual double minimalY() const;
    virtual double maximalX() const;
    virtual double maximalY() const;

    void setClosed(bool closed);
    bool isClosed() const;

    virtual std::string toConducteoElement() const;

private:

    bool intersects(const Line *line1, const Line *line2) const;
    std::vector<Point2d> intersectionPoint(const Line *line1, const Line *line2) const;
    bool pointOnCorner(const Point2d &point) const;
    bool equivalentPoints(const Point2d &p1, const Point2d &p2) const;
    double pointDistance(const Point2d &p1, const Point2d &p2) const;
    double pointDistance(const Line *line, const Point2d &point) const;
    bool containsPoint(const Line *line, const Point2d &point) const;
    std::vector<Point2d> removeDuplicates(const std::vector<Point2d> &points) const;

    bool    _closed;
    
};

}

#endif
