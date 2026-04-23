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

#ifndef VOLUME_H
#define VOLUME_H

#include <geometry/Point.h>
#include <vector>
#include <map>

class ErrorHandler;
class Material;
class Model;
class Path;
class Line;
class Cell;

class Volume: public std::vector<Path*>
{

public:

    Volume();
    Volume(const Volume &volume);
    virtual ~Volume();

    void operator=(const Volume &volume);
    void copyFrom(const Volume &volume);

    bool check(ErrorHandler *error);
    void clean();

    void setMaterial(Material *material);
    void setCellSize(double size, ErrorHandler *error);
    void setSelected(bool selected);
    void setId(const std::string &id);

    Material *material() const;
    double cellsize() const;
    bool selected() const;
    std::string id() const;

    bool isClosed() const;
    bool isSelfIntersected() const;
    bool containsPoint(const Point &point) const;
    bool pointOnBoundaries(const Point &point) const;
    bool pointOnCorner(const Point &point) const;
    bool containsPath(Path *path) const;
    void close();
    bool isRectangle() const;

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;
    Point middle() const;

    std::vector<Line*> getLines() const;
    Line *nearLine(const Point &p);
    void translate(double dx, double dy);
    void translatePoint(const Point &point, double dx, double dy);
    void translateBorder(const Point &p1, const Point &p2, double dx, double dy);
    void rotate(double x, double y, double angle);

    std::string serialize() const;
    std::string serializeAsRectangle() const;

    void computeLines();
    std::vector<Cell*> createCells(bool humidityMode, ErrorHandler *error) const;
    bool intersectionWithMesh(Cell *cell) const;

    void initRefinment();
    void increaseRefinment();

    void setModelForInternalVolumes(Model *model);
    bool intersectionWithVolume(Volume *volume) const;
    bool isIntoCell(Cell *cell) const;
    void computeBounds();

    static Volume *createFromXml(const std::string &xml, ErrorHandler *error, std::map<std::string, std::string> &swap);
    std::vector<Volume*> split() const;
    Path *nextLine(Path *line, const std::vector<Path*> &lines, int &idx) const;
    Path *firstLine(const std::vector<Path*> &lines, int &idx) const;

    bool containsArc() const;

    void addPoint(const Point &point);
    void removePoint(const Point &point);

    // Optimization.
    std::vector<Cell*>      _internalCells;
    std::vector<Volume*>    _internalVolumes;
    double                  _xmin;
    double                  _xmax;
    double                  _ymin;
    double                  _ymax;

private:

    bool isClosed_1() const;
    bool isClosed_2() const;

    double                  _cellSize;
    bool                    _selected;
    double                  _refinment;
    std::string             _id;
    std::string             _materialId;
    std::vector<Line*>      _lines;

};

#endif
