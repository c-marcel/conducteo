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

#ifndef PATH_H
#define PATH_H

#include <geometry/Point.h>
#include <vector>
#include <string>

class ErrorHandler;

class Path
{

public:

    enum Type
    {
        LinePath,
        ArcPath
    };

    Path();
    virtual ~Path();

    virtual bool check(ErrorHandler *error);
    virtual bool isVertical() const;

    virtual void setBegin(const Point &point, ErrorHandler *error)=0;
    virtual void setEnd(const Point &point, ErrorHandler *error)=0;

    virtual Type type() const=0;
    virtual Point begin() const=0;
    virtual Point end() const=0;

    virtual bool containsPoint(const Point &point) const=0;
    virtual bool containsPath(const Path *path) const=0;
    virtual double distance(const Point &point) const=0;

    virtual double xMin() const=0;
    virtual double xMax() const=0;
    virtual double yMin() const=0;
    virtual double yMax() const=0;

    virtual double angle() const;

    virtual void translate(double dx, double dy)=0;

    virtual std::string serialize() const=0;

    static Path *createFromXml(const std::string &xml, ErrorHandler *error);

};

#endif
