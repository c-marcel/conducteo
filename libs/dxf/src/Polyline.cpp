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

#include <Polyline.h>
#include <time.h>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

#define POINT_DISTANCE_PRECISION 1.0e-12

using namespace DxfReader;

Polyline::Polyline():
    _closed(false)
{
}

Polyline::~Polyline()
{
}

Entity::Type Polyline::getType() const
{
    return TypePolyline;
} 

bool Polyline::containsPoint(double x, double y, double epsilon) const
{
    for (unsigned int i=0 ; i<size()-1 ; i++)
    {
        const Vertex &v1=at(i);
        const Vertex &v2=at(i+1);

        Line line;
        Point2d p1;
        p1.setX(v1.x());
        p1.setY(v1.y());

        Point2d p2;
        p2.setX(v2.x());
        p2.setY(v2.y());

        line.setBegin(p1);
        line.setEnd(p2);

        if (line.containsPoint(x, y, epsilon))
            return true;
    }

    if (_closed && size()>2)
    {
        const Vertex &v1=at(size()-1);
        const Vertex &v2=at(0);

        Line line;
        Point2d p1;
        p1.setX(v1.x());
        p1.setY(v1.y());

        Point2d p2;
        p2.setX(v2.x());
        p2.setY(v2.y());

        line.setBegin(p1);
        line.setEnd(p2);

        if (line.containsPoint(x, y, epsilon))
            return true;
    }

    std::vector<Vertex> vertex=*this;
    if (_closed && size()>2)
        vertex.push_back(at(0));

    if (vertex.size()>2)
    {
        double xmin=minimalX();
        double xmax=maximalX();
        double ymin=minimalY();
        double ymax=maximalY();

        Point2d pmin;
        pmin.setX(xmin);
        pmin.setY(ymin);

        Point2d pmax;
        pmax.setX(xmax);
        pmax.setY(ymax);

        double dx=pmax.x()-pmin.x();
        double dy=pmax.y()-pmin.y();
        double d=std::max(dx, dy);

        double theta=rand()/double(RAND_MAX)*2.0*M_PI;

        Point2d T;
        T.setX(pmin.x()+0.5*dx+5.0*d*cos(theta));
        T.setY(pmin.y()+0.5*dy+5.0*d*sin(theta));

        Point2d point;
        point.setX(x);
        point.setY(y);

        Line segment;
        segment.setBegin(point);
        segment.setEnd(T);

        int n=0;
        for (unsigned int i=0 ; i<vertex.size()-1 ; i++)
        {
            Vertex v1=vertex.at(i);
            Vertex v2=vertex.at(i+1);

            Line line;

            Point2d p1;
            p1.setX(v1.x());
            p1.setY(v1.y());

            Point2d p2;
            p2.setX(v2.x());
            p2.setY(v2.y());

            line.setBegin(p1);
            line.setEnd(p2);

            if (intersects(&segment, &line))
            {
                std::vector<Point2d> p=intersectionPoint(&segment, &line);
                n+=p.size();

                // Point is on a volume corner: cancel et restart.
                for (unsigned int j=0 ; j<p.size() ; j++)
                    if (pointOnCorner(p.at(j)))
                        return containsPoint(x, y, epsilon);
            }
        }

        if (n%2==0)
            return false;
        return true;
    }
    return false;
}

double Polyline::minimalX() const
{
    double v_min=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
        if (at(i).x()<v_min || i==0)
            v_min=at(i).x();
    return v_min;
}

double Polyline::minimalY() const
{
    double v_min=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
        if (at(i).y()<v_min || i==0)
            v_min=at(i).y();
    return v_min;
}

double Polyline::maximalX() const
{
    double v_max=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
        v_max=std::max(v_max, at(i).x());
    return v_max;
}

double Polyline::maximalY() const
{
    double v_max=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
        v_max=std::max(v_max, at(i).y());
    return v_max;
}

void Polyline::setClosed(bool closed)
{
    _closed=closed;
}

bool Polyline::isClosed() const
{
    return _closed;
}

std::string Polyline::toConducteoElement() const
{
    std::string xml;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Vertex v1=at(i);
        Vertex v2;
        if (i<size()-1)
            v2=at(i+1);
        else
            v2=at(0);

        xml+="<line>";
        xml+="<begin>";
        xml+=v1.toConducteoElement();
        xml+="</begin>";

        xml+="<end>";
        xml+=v2.toConducteoElement();
        xml+="</end>";
        xml+="</line>";
    }
    return xml;
}

bool Polyline::intersects(const Line *line1, const Line *line2) const
{
    std::vector<Point2d> points=intersectionPoint(line1, line2);
    return (points.size()>0);
}

bool Polyline::equivalentPoints(const Point2d &p1, const Point2d &p2) const
{
    return (pointDistance(p1, p2)<POINT_DISTANCE_PRECISION);
}

double Polyline::pointDistance(const Point2d &p1, const Point2d &p2) const
{
    return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y()));
}

std::vector<Point2d> Polyline::intersectionPoint(const Line *line1, const Line *line2) const
{
    std::vector<Point2d> points;

    if (!line1)
        return points;
    if (!line2)
        return points;

    // Optimization.
    if (line1->maximalX()+1.0e-4<line2->minimalX())
        return points;
    if (line1->maximalY()+1.0e-4<line2->minimalY())
        return points;
    if (line2->maximalY()+1.0e-4<line1->minimalY())
        return points;
    if (line2->maximalX()+1.0e-4<line1->minimalX())
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

        Point2d p;
        p.setX(x);
        p.setY(y);
        points.push_back(p);
    }

    // Keep points that are near the 2 segments.
    std::vector<Point2d> points2;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point2d &p=points.at(i);
        if (!containsPoint(line1, p))
            continue;
        if (!containsPoint(line2, p))
            continue;
        points2.push_back(p);
    }
    return removeDuplicates(points2);
}

bool Polyline::containsPoint(const Line *line, const Point2d &point) const
{
    double distance=pointDistance(line, point);
    
    // Assume point is on segment if distance between both is null.
    if (std::fabs(distance)<POINT_DISTANCE_PRECISION)
        return true;
    return false;
}

std::vector<Point2d> Polyline::removeDuplicates(const std::vector<Point2d> &points) const
{
    std::vector<Point2d> p;
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

bool Polyline::pointOnCorner(const Point2d &point) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Vertex v=at(i);

        Point2d p;
        p.setX(v.x());
        p.setY(v.y());

        if (equivalentPoints(p, point))
            return true;
    }
    return false;
}

double Polyline::pointDistance(const Line *line, const Point2d &point) const
{
    if (!line)
        return 0.0;

    double x1 = line->begin().x();
    double y1 = line->begin().y();

    double x2 = line->end().x();
    double y2 = line->end().y();

    double x = point.x();
    double y = point.y();

    double dx = x2 - x1;
    double dy = y2 - y1;

    double len = std::hypot(dx, dy);

    if (len < 1e-12)
    {
        double dxp = x - x1;
        double dyp = y - y1;
        return std::hypot(dxp, dyp);
    }

    double t = ((x - x1) * dx + (y - y1) * dy) / (len * len);

    double projX, projY;

    if (t <= 0.0)
    {
        projX = x1;
        projY = y1;
    }

    else if (t >= 1.0)
    {
        projX = x2;
        projY = y2;
    }

    else
    {
        projX = x1 + t * dx;
        projY = y1 + t * dy;
    }

    return std::hypot(x - projX, y - projY);
}
