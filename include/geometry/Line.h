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

#ifndef LINE_H
#define LINE_H

#include <geometry/Point.h>
#include <geometry/Path.h>

class ErrorHandler;

class Line: public Path
{

public:

    Line();
    virtual ~Line();
    Line(const Line &line);

    Type type() const;
    void setBegin(const Point &point, ErrorHandler *error);
    void setEnd(const Point &point, ErrorHandler *error);
    virtual bool isVertical() const;

    std::vector<Line*> cut(const Point &point) const;

    Point middlePoint() const;
    bool containsPoint(const Point &point) const;
    bool containsPath(const Path *path) const;
    Point nearMiddlePoint(bool type) const;
    double distance(const Point &point) const;

    bool isEquivalentTo(const Path *path) const;

    Point begin() const;
    Point end() const;

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

    double angle() const;
    double length() const;

    void translate(double dx, double dy);
    void setInitialPath(Path *path);
    Path *initialPath() const;

    std::string serialize() const;

    static Line *createFromXml(const std::string &xml, ErrorHandler *error);

private:

    Path   *_initialPath;

    Point   _begin;
    Point   _end;

};

#endif
