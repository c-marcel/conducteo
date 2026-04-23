// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2020, Cl�ment MARCEL.
//
// This file is part of conducte� software.
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

#include <geometry/GeometryTools.h>
#include <geometry/transformations/Transformations.h>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <geometry/transformations/Translation.h>
#include <boost/geometry/geometries/segment.hpp>
#include <geometry/transformations/Rotation.h>
#include <boost/geometry/geometries/point.hpp>
#include <geometry/transformations/Scaling.h>
#include <geometry/Line.h>
#include <geometry/Arc.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define POINT_DISTANCE_PRECISION 1.0e-12

typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> BoostPoint;
typedef boost::geometry::model::segment<BoostPoint> BoostSegment;

bool GeometryTools::intersects(const Line *line1, const Line *line2)
{
    std::vector<Point> points=intersectionPoint(line1, line2);
    return (points.size()>0);
}

bool GeometryTools::intersectsWithoutBounds(const Line *line1, const Line *line2)
{
    std::vector<Point> points=intersectionPointWithoutBounds(line1, line2);
    return (points.size()>0);
}

bool GeometryTools::intersectsWithoutCorner(const Line *line1, const Line *line2)
{
    std::vector<Point> points=intersectionPointWithoutCorner(line1, line2);
    return (points.size()>0);
}

bool GeometryTools::containsPoint(const Line *line, const Point &point)
{
    double distance=pointDistance(line, point);
    
    // Assume point is on segment if distance between both is null.
    if (std::fabs(distance)<POINT_DISTANCE_PRECISION)
        return true;
    return false;
}

double GeometryTools::pointDistance(const Point &p1, const Point &p2)
{
    return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y()));
}

bool GeometryTools::equivalentPoints(const Point &p1, const Point &p2)
{
    return (pointDistance(p1, p2)<POINT_DISTANCE_PRECISION);
}

double GeometryTools::pointDistance(const Line *line, const Point &point)
{
    if (!line)
        return false;

    BoostPoint line_begin(line->begin().x(), line->begin().y());
    BoostPoint line_end(line->end().x(), line->end().y());

    BoostSegment line_segment(line_begin, line_end);

    BoostPoint p(point.x(), point.y());

    return boost::geometry::distance(line_segment, p);
}

std::vector<Point> GeometryTools::intersectionPoint(const Line *line1, const Line *line2)
{
    std::vector<Point> points;

    if (!line1)
        return points;
    if (!line2)
        return points;

    // Optimization.
    if (line1->xMax()+1.0e-4<line2->xMin())
        return points;
    if (line1->yMax()+1.0e-4<line2->yMin())
        return points;
    if (line2->yMax()+1.0e-4<line1->yMin())
        return points;
    if (line2->xMax()+1.0e-4<line1->xMin())
        return points;

    // Consider 4 bounding points.
    points.push_back(line1->begin());
    points.push_back(line1->end());
    points.push_back(line2->begin());
    points.push_back(line2->end());

    // Add intersection point if existing.
    double a=line1->begin().y()-line1->end().y();
    double b=line1->end().x()-line1->begin().x();

    double ap=line2->begin().y()-line2->end().y();
    double bp=line2->end().x()-line2->begin().x();
    
    double delta=b*ap-bp*a;
    if (std::fabs(delta)>1.0e-12)
    {
        double c=(line1->begin().x()-line1->end().x())*line1->begin().y()+(line1->end().y()-line1->begin().y())*line1->begin().x();
        double cp=(line2->begin().x()-line2->end().x())*line2->begin().y()+(line2->end().y()-line2->begin().y())*line2->begin().x();

        double y=(cp*a-c*ap)/delta;
        double x;
        if (std::fabs(a)>std::fabs(ap))
            x=(-b*y-c)/a;
        else
            x=(-bp*y-cp)/ap;

        Point p;
        p.setX(x);
        p.setY(y);
        points.push_back(p);
    }

    // Keep points that are near the 2 segments.
    std::vector<Point> points2;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point &p=points.at(i);
        if (!containsPoint(line1, p))
            continue;
        if (!containsPoint(line2, p))
            continue;
        points2.push_back(p);
    }
    return removeDuplicates(points2);
}

std::vector<Point> GeometryTools::removeDuplicates(const std::vector<Point> &points)
{
    std::vector<Point> p;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        bool defined=false;
        for (unsigned int j=0 ; j<p.size() ; j++)
        {
            if (equivalentPoints(p.at(j), points.at(i)))
            {
                defined=true;
                break;
            }
        }
        if (defined)
            continue;
        p.push_back(points.at(i));
    }

    return p;
}

std::vector<Point> GeometryTools::intersectionPointWithoutBounds(const Line *line1, const Line *line2)
{
    std::vector<Point> points;

    if (!line1)
        return points;
    if (!line2)
        return points;

    // Optimization.
    if (line1->xMax()+1.0e-4<line2->xMin())
        return points;
    if (line1->yMax()+1.0e-4<line2->yMin())
        return points;
    if (line2->yMax()+1.0e-4<line1->yMin())
        return points;
    if (line2->xMax()+1.0e-4<line1->xMin())
        return points;

    // Add intersection point if existing.
    double a=line1->begin().y()-line1->end().y();
    double b=line1->end().x()-line1->begin().x();

    double ap=line2->begin().y()-line2->end().y();
    double bp=line2->end().x()-line2->begin().x();
    
    double delta=b*ap-bp*a;
    if (std::fabs(delta)>1.0e-12)
    {
        double c=(line1->begin().x()-line1->end().x())*line1->begin().y()+(line1->end().y()-line1->begin().y())*line1->begin().x();
        double cp=(line2->begin().x()-line2->end().x())*line2->begin().y()+(line2->end().y()-line2->begin().y())*line2->begin().x();

        double y=(cp*a-c*ap)/delta;
        double x;
        if (std::fabs(a)>std::fabs(ap))
            x=(-b*y-c)/a;
        else
            x=(-bp*y-cp)/ap;

        Point p;
        p.setX(x);
        p.setY(y);
        points.push_back(p);
    }

    // Keep points that are near the 2 segments.
    std::vector<Point> points2;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point &p=points.at(i);
        if (!containsPoint(line1, p))
            continue;
        if (!containsPoint(line2, p))
            continue;

        // Exclude bounds.
        if (equivalentPoints(p, line1->begin()))
            continue;
        if (equivalentPoints(p, line1->end()))
            continue;
        if (equivalentPoints(p, line2->begin()))
            continue;
        if (equivalentPoints(p, line2->end()))
            continue;

        points2.push_back(p);
    }
    return points2;
}

std::vector<Point> GeometryTools::intersectionPointWithoutCorner(const Line *line1, const Line *line2)
{
    std::vector<Point> points;

    if (!line1)
        return points;
    if (!line2)
        return points;

    // Optimization.
    if (line1->xMax()+1.0e-4<line2->xMin())
        return points;
    if (line1->yMax()+1.0e-4<line2->yMin())
        return points;
    if (line2->yMax()+1.0e-4<line1->yMin())
        return points;
    if (line2->xMax()+1.0e-4<line1->xMin())
        return points;

    // Add intersection point if existing.
    double a=line1->begin().y()-line1->end().y();
    double b=line1->end().x()-line1->begin().x();

    double ap=line2->begin().y()-line2->end().y();
    double bp=line2->end().x()-line2->begin().x();
    
    double delta=b*ap-bp*a;
    if (std::fabs(delta)>1.0e-12)
    {
        double c=(line1->begin().x()-line1->end().x())*line1->begin().y()+(line1->end().y()-line1->begin().y())*line1->begin().x();
        double cp=(line2->begin().x()-line2->end().x())*line2->begin().y()+(line2->end().y()-line2->begin().y())*line2->begin().x();

        double y=(cp*a-c*ap)/delta;
        double x;
        if (std::fabs(a)>std::fabs(ap))
            x=(-b*y-c)/a;
        else
            x=(-bp*y-cp)/ap;

        Point p;
        p.setX(x);
        p.setY(y);
        points.push_back(p);
    }

    // Keep points that are near the 2 segments.
    std::vector<Point> points2;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point &p=points.at(i);
        if (!containsPoint(line1, p))
            continue;
        if (!containsPoint(line2, p))
            continue;

        // Exclude corner.
        if (equivalentPoints(p, line1->begin()) && equivalentPoints(p, line2->begin()))
            continue;
        if (equivalentPoints(p, line1->begin()) && equivalentPoints(p, line2->end()))
            continue;

        if (equivalentPoints(p, line1->end()) && equivalentPoints(p, line2->begin()))
            continue;
        if (equivalentPoints(p, line1->end()) && equivalentPoints(p, line2->end()))
            continue;

        points2.push_back(p);
    }
    return removeDuplicates(points2);
}

Point GeometryTools::ellipsePointAtAngle(double a, double b, double theta)
{
    // Bounds angle between [0 ; 360].
    while (theta<0.0)
        theta+=360.0;
    while (theta>360.0)
        theta-=360.0;

    // Case of angle is 90� or 270�.
    if (theta==90.0)
    {
        Point p;
        p.setX(0.0);
        p.setY(b);
        return p;
    }
    if (theta==90.0)
    {
        Point p;
        p.setX(0.0);
        p.setY(-b);
        return p;
    }

    // Compute x^2 & y^2.
    double ac=a*a;
    double bc=b*b;
    double tantheta=tan(theta/180.0*M_PI);
    double tanthetac=tantheta*tantheta;

    double xc=1.0/(1.0/ac+tanthetac/bc);
    double yc=(1.0-xc/ac)*bc;

    // Fix numerical zero.
    if (fabs(xc)<1.0e-12)
        xc=0.0;

    if (fabs(yc)<1.0e-12)
        yc=0.0;

    Point p;

    // Right part.
    if ((0<=theta && theta<90.0) || (theta>270.0 && theta<=360.0))
        p.setX(sqrt(xc));
    
    // Left part.
    else
        p.setX(-sqrt(xc));

    // Top part.
    if (0<=theta && theta<=180.0)
        p.setY(sqrt(yc));

    // Bottom part.
    else
        p.setY(-sqrt(yc));

    return p;
}

bool GeometryTools::intersects(const Line *line1, const Arc *arc1)
{
    for (unsigned int i=0 ; i<arc1->size() ; i++)
    {
        Line *line2=arc1->at(i);
        if (intersects(line1, line2))
            return true;
    }
    return false;
}

std::vector<Point> GeometryTools::intersectionPoint(const Line *line1, const Arc *arc1)
{
    std::vector<Point> points;
    for (unsigned int i=0 ; i<arc1->size() ; i++)
    {
        Line *line2=arc1->at(i);
        std::vector<Point> p=intersectionPoint(line1, line2);
        for (unsigned int j=0 ; j<p.size() ; j++)
            points.push_back(p.at(j));
    }
    return points;
}

bool GeometryTools::containsPoint(const Arc *arc, const Point &point)
{
    for (unsigned int i=0 ; i<arc->size() ; i++)
    {
        Line *line2=arc->at(i);
        if (line2->containsPoint(point))
            return true;
    }
    return false;
}

double GeometryTools::scalarOrthogonalProduct(const Line *line1, const Line *line2)
{
    // Unit vector for line 1.
    double dx1=line1->end().x()-line1->begin().x();
    double dy1=line1->end().y()-line1->begin().y();
    double d1=sqrt(dx1*dx1+dy1*dy1);
    dx1=dx1/d1;
    dy1=dy1/d1;

    // Unit vector for line 2.
    double dx2=line2->end().x()-line2->begin().x();
    double dy2=line2->end().y()-line2->begin().y();
    double d2=sqrt(dx2*dx2+dy2*dy2);
    dx2=dx2/d2;
    dy2=dy2/d2;

    double dx3=-dy2;
    double dy3=dx2;

    return (dx1*dx3+dy1*dy3);
}

double GeometryTools::scalarProduct(const Line *line1, const Line *line2)
{
    // Unit vector for line 1.
    double dx1=line1->end().x()-line1->begin().x();
    double dy1=line1->end().y()-line1->begin().y();
    double d1=sqrt(dx1*dx1+dy1*dy1);
    dx1=dx1/d1;
    dy1=dy1/d1;

    // Unit vector for line 2.
    double dx2=line2->end().x()-line2->begin().x();
    double dy2=line2->end().y()-line2->begin().y();
    double d2=sqrt(dx2*dx2+dy2*dy2);
    dx2=dx2/d2;
    dy2=dy2/d2;

    return (dx1*dx2+dy1*dy2);
}

Point GeometryTools::orthogonalProjection(Line *line, const Point &point)
{
    double distance=pointDistance(line, point);

    double d1x=point.x()-line->begin().x();
    double d1y=point.y()-line->begin().y();
    double delta=sqrt(d1x*d1x+d1y*d1y-distance*distance);

    double dx1=line->end().x()-line->begin().x();
    double dy1=line->end().y()-line->begin().y();
    double d1=sqrt(dx1*dx1+dy1*dy1);
    dx1=dx1/d1;
    dy1=dy1/d1;

    if (delta>d1)
        delta=d1;
    if (delta<0.0)
        delta=0.0;

    double x=line->begin().x()+delta*dx1;
    double y=line->begin().y()+delta*dy1;

    Point p;
    p.setX(x);
    p.setY(y);

    return p;
}

Line *GeometryTools::merge(Line *line1, Line *line2)
{
    // Get points.
    Point common_point=line1->begin();
    Point point1=line1->end();

    if (!equivalentPoints(common_point, line2->begin()) && !equivalentPoints(common_point, line2->end()))
    {
        common_point=line1->end();
        point1=line1->begin();
    }

    Point point2=line2->begin();
    if (equivalentPoints(point2, common_point))
        point2=line2->end();

    Line line;
    line.setBegin(point1, 0);
    line.setEnd(point2, 0);

    if (!line.containsPoint(common_point))
        return 0;

    // Merge lines.
    Line *l=new Line;
    l->setBegin(point1, 0);
    l->setEnd(point2, 0);

    return l;
}
