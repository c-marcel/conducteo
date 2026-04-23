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

#include <numeric/Cell.h>
#include <geometry/GeometryTools.h>
#include <physics/Material.h>
#include <geometry/Line.h>
#include <ctime>

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#ifdef __linux__
#include <stdlib.h>
#endif

#ifdef __APPLE__
#include <cstdlib>
#endif

using namespace std;

Cell::Cell():
    _horizontalConductivity(1.0),
    _verticalConductivity(1.0),
    _x(0.0),
    _y(0.0),
    _size(0.0),
    _temperature(0.0),
    _neighbor1(0),
    _neighbor2(0),
    _neighbor3(0),
    _neighbor4(0),
    _airCellType(0),
    _rh(0.0),
    _material(0)
{
}

Cell::~Cell()
{
    clean();
    if (_neighbor1)
        _neighbor1->_neighbor3=0;
    if (_neighbor2)
        _neighbor2->_neighbor4=0;
    if (_neighbor3)
        _neighbor3->_neighbor1=0;
    if (_neighbor4)
        _neighbor4->_neighbor2=0;
}

bool Cell::isInternal() const
{
    if (!_neighbor1)
        return false;
    if (!_neighbor2)
        return false;
    if (!_neighbor3)
        return false;
    if (!_neighbor4)
        return false;
    return true;
}

Cell::Cell(const Cell &cell)
{
    _x                      = cell._x;
    _y                      = cell._y;
    _size                   = cell._size;
    _horizontalConductivity = cell._horizontalConductivity;
    _verticalConductivity   = cell._verticalConductivity;
    _temperature            = cell._temperature;
    _center                 = cell._center;

    _xmin                   = cell._xmin;
    _xmax                   = cell._xmax;
    _ymin                   = cell._ymin;
    _ymax                   = cell._ymax;

    _rh                     = cell._rh;
    _material               = cell._material;

    _neighbor1              = 0;
    _neighbor2              = 0;
    _neighbor3              = 0;
    _neighbor4              = 0;
    _matrixIdx              = cell._matrixIdx;
    _airCellType            = cell._airCellType;

    _neighbors.resize(0);

    for (unsigned int i=0 ; i<cell.size() ; i++)
        push_back(new Line(*cell.at(i)));
}

void Cell::clean()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
    resize(0);
}

void Cell::create(double x, double y, double cellSize)
{
    clean();

    _x=x;
    _y=y;
    _size=cellSize;

    Point p1;
    p1.setX(x);
    p1.setY(y);

    Point p2;
    p2.setX(x+cellSize);
    p2.setY(y);

    Point p3;
    p3.setX(x+cellSize);
    p3.setY(y+cellSize);

    Point p4;
    p4.setX(x);
    p4.setY(y+cellSize);

    Line *path1=new Line;
    path1->setBegin(p1, 0);
    path1->setEnd(p2, 0);
    push_back(path1);

    Line *path2=new Line;
    path2->setBegin(p2, 0);
    path2->setEnd(p3, 0);
    push_back(path2);

    Line *path3=new Line;
    path3->setBegin(p3, 0);
    path3->setEnd(p4, 0);
    push_back(path3);

    Line *path4=new Line;
    path4->setBegin(p4, 0);
    path4->setEnd(p1, 0);
    push_back(path4);

    _center.setX(x+0.5*cellSize);
    _center.setY(y+0.5*cellSize);
}

Point Cell::squareMeshMiddle() const
{
    Point m;
    m.setX(_x+0.5*_size);
    m.setY(_y+0.5*_size);
    return m;
}

void Cell::useHumidityData()
{
    if (!_material)
        return;

    _horizontalConductivity = 1.0 / _material->mu();
    _verticalConductivity   = _horizontalConductivity;
}

bool Cell::squareMeshContainsPoint(const Point &point) const
{
    double x=point.x();
    double y=point.y();

    if (x<_x)
        return false;
    if (y<_y)
        return false;

    if (x>_x+_size)
        return false;
    if (y>_y+_size)
        return false;

    return true;
}

double Cell::x() const
{
    return _x;
}

double Cell::y() const
{
    return _y;
}

double Cell::cellSize() const
{
    return _size;
}

void Cell::setHorizontalConductivity(double conductivity)
{
    _horizontalConductivity=conductivity;
}

void Cell::setVerticalConductivity(double conductivity)
{
    _verticalConductivity=conductivity;
}

void Cell::setMaterial(Material *material)
{
    _material = material;
}

double Cell::conductivity(const Line *direction) const
{
    // Unit vector for line.
    double dx1=direction->end().x()-direction->begin().x();
    double dy1=direction->end().y()-direction->begin().y();
    double d1=sqrt(dx1*dx1+dy1*dy1);
    dx1=fabs(dx1/d1);
    dy1=fabs(dy1/d1);

    // Scalar product.
    return dx1*_horizontalConductivity+dy1*_verticalConductivity;
}

double Cell::horizontalConductivity() const
{
    return _horizontalConductivity;
}

double Cell::verticalConductivity() const
{
    return _verticalConductivity;
}

void Cell::computeCenter()
{
    double x=0.0;
    double y=0.0;
    double total=0.0;
    
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        Point b=line->begin();
        Point e=line->end();

        double dx=e.x()-b.x();
        double dy=e.y()-b.y();
        double d=sqrt(dx*dx+dy*dy);

        Point m=line->middlePoint();

        x+=m.x()*d;
        y+=m.y()*d;
        total+=d;
    }

    x=x/total;
    y=y/total;

    _center.setX(x);
    _center.setY(y);
}

Point Cell::center() const
{
    return _center;
}

double Cell::commonArea(Line *line1) const
{
    // Optimizations.
    // Line is on the left of the cell. Etc.
    if (line1->xMax()<xMin()-1.0e-4)
        return 0.0;
    if (line1->xMin()>xMax()+1.0e-4)
        return 0.0;
    if (line1->yMax()<yMin()-1.0e-4)
        return 0.0;
    if (line1->yMin()>yMax()+1.0e-4)
        return 0.0;

    double area=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line2=at(i);
            
        std::vector<Point> points=GeometryTools::intersectionPoint(line1, line2);
        if (points.size()<2)
            continue;

        double dx=points.at(1).x()-points.at(0).x();
        double dy=points.at(1).y()-points.at(0).y();

        area+=sqrt(dx*dx+dy*dy);
    }
    return area;
}

void Cell::setTemperature(double temperature)
{
    _temperature=temperature;
}

void Cell::setRh(double rh)
{
    _rh = rh;
}

double Cell::temperature() const
{
    return _temperature;
}

double Cell::rh() const
{
    return _rh;
}

Material *Cell::material() const
{
    return _material;
}

double Cell::xMin() const
{
    return _xmin;
}

double Cell::xMax() const
{
    return _xmax;
}

double Cell::yMin() const
{
    return _ymin;
}

double Cell::yMax() const
{
    return _ymax;
}

bool Cell::pointOnBoundary(const Point &point) const
{
    for (unsigned int i=0 ; i<size() ; i++)
        if (at(i)->containsPoint(point))
            return true;
    return false;
}

bool Cell::pointOnCorner(const Point &point) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        Point b=line->begin();
        Point e=line->end();
        if (GeometryTools::equivalentPoints(point, e))
            return true;
        if (GeometryTools::equivalentPoints(point, b))
            return true;
    }
    return false;
}

void Cell::computeBounds()
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->xMin()<_xmin || i==0)
            _xmin=at(i)->xMin();
    }
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->xMax()>_xmax || i==0)
            _xmax=at(i)->xMax();
    }
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->yMin()<_ymin || i==0)
            _ymin=at(i)->yMin();
    }
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->yMax()>_ymax || i==0)
            _ymax=at(i)->yMax();
    }
}

void Cell::closeCell()
{
    if (size()<=1)
        return;

    for (unsigned int i=0 ; i<size()-1 ; i++)
    {
        Line *line1=at(i);
        Line *old_line=at(i+1);
        Point end=line1->end();
        for (unsigned int j=i+1 ; j<size() ; j++)
        {
            Line *line2=at(j);
            Point b=line2->begin();
            Point e=line2->end();

            if (GeometryTools::equivalentPoints(end, b))
            {
                at(i+1)=line2;
                at(j)=old_line;
                break;
            }

            if (GeometryTools::equivalentPoints(end, e))
            {
                line2->setBegin(e, 0);
                line2->setEnd(b, 0);
                at(i+1)=line2;
                at(j)=old_line;
                break;
            }
        }
    }
}

bool Cell::containsPoint(const Point &point) const
{
    // Optimizations.
    if (point.x()<xMin())
        return false;
    if (point.x()>xMax())
        return false;
    if (point.y()<yMin())
        return false;
    if (point.y()>yMax())
        return false;

    // If point is a volume defined point, return true.
    if (pointOnBoundary(point))
        return true;

    double xmin=xMin();
    double xmax=xMax();
    double ymin=yMin();
    double ymax=yMax();

    Point pmin;
    pmin.setX(xmin);
    pmin.setY(ymin);

    Point pmax;
    pmax.setX(xmax);
    pmax.setY(ymax);

    double dx=pmax.x()-pmin.x();
    double dy=pmax.y()-pmin.y();
    double d=std::max(dx, dy);

    double theta=rand()/double(RAND_MAX)*2.0*M_PI;

    Point T;
    T.setX(pmin.x()+0.5*dx+5.0*d*cos(theta));
    T.setY(pmin.y()+0.5*dy+5.0*d*sin(theta));

    Line segment;
    segment.setBegin(point, 0);
    segment.setEnd(T, 0);

    int n=0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        if (GeometryTools::intersects(&segment, line))
        {
            std::vector<Point> p=GeometryTools::intersectionPoint(&segment, line);
            n+=p.size();

            // Point is on a volume corner: cancel et restart.
            for (unsigned int j=0 ; j<p.size() ; j++)
                if (pointOnCorner(p.at(j)))
                    return containsPoint(point);
        }
    }

    if (n%2==0)
        return false;
    return true;
}

bool Cell::hasSurface() const
{
    double dx=xMax()-xMin();
    double dy=yMax()-yMin();
    if (dx<1.0e-9)
        return false;
    if (dy<1.0e-9)
        return false;
    return true;
}

double Cell::area() const
{
    double a=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        a+=0.5*(line->begin().x()+line->end().x())*(line->begin().y()-line->end().y());
    }
    return fabs(a);
}

bool Cell::isClosed() const
{
    // Volume is closed if number of surfaces & number of unique points
    // are the same.
    unsigned int n_surfaces=size();
    std::vector<Point> points;

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Path *path=at(i);
        
        Point begin=path->begin();
        Point end=path->end();

        points.push_back(begin);
        points.push_back(end);
    }

    std::vector<Point> points2;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point &p1=points.at(i);
        bool defined=false;
        for (unsigned int j=0 ; j<points2.size() ; j++)
        {
            Point &p2=points2.at(j);
            if (GeometryTools::equivalentPoints(p1, p2))
            {
                defined=true;
                break;
            }
        }
        if (!defined)
            points2.push_back(p1);
    }

    return (points2.size()==n_surfaces);
}

bool Cell::mergeTwoLines()
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line1=at(i);
        Line *line2=at(0);
        if (i<size()-1)
            line2=at(i+1);

        Line *m=GeometryTools::merge(line1, line2);
        if (!m)
            continue;

        at(i)=m;

        unsigned int idx=0;
        if (i<size()-1)
            idx=i+1;
        erase(begin()+idx);
        return true;
    }
    return false;
}

void Cell::removeUnusedPoints()
{
    if (size()<3)
        return;

    while (mergeTwoLines())
    {
    }
}
