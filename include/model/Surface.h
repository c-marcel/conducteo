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

#ifndef SURFACE_H
#define SURFACE_H

#include <geometry/Line.h>
#include <vector>
#include <string>
#include <map>

class BoundaryCondition;
class ErrorHandler;
class Environment;
class Path;

class Surface: public std::vector<Line*>
{

public:

    Surface();
    Surface(const Surface &surface);
    virtual ~Surface();

    bool check(ErrorHandler *error);

    void setBoundaryCondition(BoundaryCondition *bc);
    void setEnvironment(Environment *env);
    void setSelected(bool selected);

    bool containsPoint(const Point &point) const;
    double pointDistance(const Point &point) const;
    bool haveCommonPoints(Surface *surface) const;

    BoundaryCondition *boundaryCondition() const;
    Environment *environment() const;
    bool selected() const;

    void setFixedTemperature(double temperature);
    void unsetFixedTemperature();
    double temperature() const;
    double length() const;

    void setFlux(double flux);
    void setMinimalTemperature(double temperature);
    void setMaximalTemperature(double temperature);
    void setMinimalTemperatureLocation(const Point &location);
    void setMaximalTemperatureLocation(const Point &location);

    double flux() const;
    double minimalTemperature() const;
    double maximalTemperature() const;
    Point minimalTemperatureLocation() const;
    Point maximalTemperatureLocation() const;

    double commonArea(Line *line) const;

    bool isVertical() const;
    bool isHorizontal() const;

    std::string serialize() const;
    std::string serializeAsV2Surface() const;

    static Surface *createFromXml(const std::string &xml, ErrorHandler *error, std::map<std::string, std::string> &swap);

    double minimalRh() const;
    void setMinimalRh(double minimalRh);

    double maximalRh() const;
    void setMaximalRh(double maximalRh);

private:
    
    void clean();

    BoundaryCondition  *_boundaryCondition;
    Environment        *_environment;

    double              _flux;
    double              _minimalTemperature;
    double              _maximalTemperature;
    double              _minimalRh;
    double              _maximalRh;

    Point               _minimalTemperatureLocation;
    Point               _maximalTemperatureLocation;

    bool                _selected;
    bool                _usedFixedTemperature;
    double              _fixedTemperature;

};

#endif
