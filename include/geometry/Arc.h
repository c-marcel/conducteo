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

#ifndef ARC_H
#define ARC_H

#include <geometry/transformations/Rotation.h>
#include <geometry/Point.h>
#include <geometry/Line.h>
#include <geometry/Path.h>
#include <vector>

class ErrorHandler;

class Arc: public Path,
           public std::vector<Line*>
{

public:

    Arc();
    Arc(const Arc &arc);
    virtual ~Arc();

    bool check(ErrorHandler *error);
    void recreateLines();

    Type type() const;
    Point begin() const;
    Point end() const;
    
    bool containsPoint(const Point &point) const;
    bool containsPath(const Path *path) const;
    double distance(const Point &point) const;

    void setRotationAngle(double angle);
    void setCenter(const Point &center);
    void setHorizontalRadius(double radius, ErrorHandler *error);
    void setVerticalRadius(double radius, ErrorHandler *error);
    void setBegin(const Point &point, ErrorHandler *error);
    void setEnd(const Point &point, ErrorHandler *error);
    void setAngle(double begin, double angle, ErrorHandler *error, bool direct=true);

    Point center() const;
    double rotationAngle() const;
    double horizontalRadius() const;
    double verticalRadius() const;

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

    void translate(double dx, double dy);

    std::string serialize() const;
    static Arc *createFromXml(const std::string &xml, ErrorHandler *error);

private:

    void clean();
    double angleAtPoint(const Point &point) const;
    bool isPointOnEllipse(const Point &point) const;

    Point       _center;
    double      _horizontalRadius;
    double      _verticalRadius;
    Point       _beginPoint;
    Point       _endPoint;
    double      _rotationAngle;

};

#endif
