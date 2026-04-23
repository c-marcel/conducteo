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

#ifndef _CELL_H
#define _CELL_H

#include <geometry/Point.h>
#include <vector>

class Material;
class Line;

class Cell: public std::vector<Line*>
{

public:

    Cell();
    Cell(const Cell &cell);
    virtual ~Cell();

    void create(double x, double y, double size);
    void setHorizontalConductivity(double conductivity);
    void setVerticalConductivity(double conductivity);
    void setMaterial(Material *material);
    void setTemperature(double temperature);
    void setRh(double rh);
    bool squareMeshContainsPoint(const Point &point) const;
    Point squareMeshMiddle() const;

    void useHumidityData();

    void computeCenter();
    Point center() const;

    double commonArea(Line *line) const;
    bool pointOnBoundary(const Point &point) const;
    bool pointOnCorner(const Point &point) const;
    bool containsPoint(const Point &point) const;

    double x() const;
    double y() const;
    double cellSize() const;
    double conductivity(const Line *direction) const;
    double horizontalConductivity() const;
    double verticalConductivity() const;
    double temperature() const;
    double rh() const;
    Material *material() const;

    void computeBounds();
    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

    void closeCell();
    void removeUnusedPoints();
    bool isClosed() const;
    void clean();
    bool hasSurface() const;
    double area() const;

    // Optimization data.
    Cell        *_neighbor1;    //< Left
    Cell        *_neighbor2;    //< Bottom
    Cell        *_neighbor3;    //< Right
    Cell        *_neighbor4;    //<Top
    unsigned int _matrixIdx;
    unsigned int _airCellType;  //< 0: no air material, 1: unventilated air, 2: partially ventilated air.
    std::vector<Cell *> _neighbors;
    bool isInternal() const;

private:

    bool mergeTwoLines();

    double  _x;
    double  _y;
    double  _size;
    double  _horizontalConductivity;
    double  _verticalConductivity;
    double  _temperature;
    Point   _center;

    double  _xmin;
    double  _xmax;
    double  _ymin;
    double  _ymax;

    // Humidity.
    double      _rh;
    Material   *_material;

};

#endif
