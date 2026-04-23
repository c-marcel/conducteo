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

#ifndef GEOMETRY_TOOLS_H
#define GEOMETRY_TOOLS_H

#include <geometry/Point.h>
#include <vector>

class Line;
class Arc;

class GeometryTools
{

public:

    static double pointDistance(const Line *line, const Point &point);
    static double pointDistance(const Point &p1, const Point &p2);
    static bool equivalentPoints(const Point &p1, const Point &p2);

    static Point ellipsePointAtAngle(double a, double b, double theta);
    static bool containsPoint(const Line *line, const Point &point);
    static bool containsPoint(const Arc *arc, const Point &point);

    static bool intersects(const Line *line1, const Line *line2);
    static std::vector<Point> intersectionPoint(const Line *line1, const Line *line2);

    static bool intersectsWithoutBounds(const Line *line1, const Line *line2);
    static std::vector<Point> intersectionPointWithoutBounds(const Line *line1, const Line *line2);

    static bool intersectsWithoutCorner(const Line *line1, const Line *line2);
    static std::vector<Point> intersectionPointWithoutCorner(const Line *line1, const Line *line2);

    static bool intersects(const Line *line1, const Arc *arc1);
    static std::vector<Point> intersectionPoint(const Line *line1, const Arc *arc1);

    static double scalarOrthogonalProduct(const Line *line1, const Line *line2);
    static double scalarProduct(const Line *line1, const Line *line2);

    static std::vector<Point> removeDuplicates(const std::vector<Point> &points);

    static Point orthogonalProjection(Line *line, const Point &point);
    static Line *merge(Line *line1, Line *line2);

};

#endif
