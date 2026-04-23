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

#include <geometry/Line.h>
#include <geometry/GeometryTools.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <geometry/Arc.h>
#include <tinyxml.h>

#define _USE_MATH_DEFINES
#include <math.h>

Line::Line():
    _initialPath(0)
{
}

Line::~Line()
{
}

Line::Line(const Line &line)
{
    _begin       = line._begin;
    _end         = line._end;
    _initialPath = 0;
}

Path::Type Line::type() const
{
    return LinePath;
}

void Line::setBegin(const Point &begin, ErrorHandler *error)
{
    _begin=begin;
}

void Line::setEnd(const Point &end, ErrorHandler *error)
{
    _end=end;
}

Point Line::begin() const
{
    return _begin;
}

Point Line::end() const
{
    return _end;
}

std::string Line::serialize() const
{
    std::string xml="<line>";
    xml+="<begin>";
    xml+=_begin.serialize();
    xml+="</begin>";

    xml+="<end>";
    xml+=_end.serialize();
    xml+="</end>";
    xml+="</line>";
    return xml;
}

Line *Line::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *line=doc.RootElement();
    if (!line)
    {
        setStaticError(error, BadLineNode);
        return 0;
    }

    if (!line->Value())
    {
        setStaticError(error, BadLineNode);
        return 0;
    }

    std::string root_name=line->Value();
    if (root_name!="line")
    {
        setStaticError(error, BadLineNode);
        return 0;
    }

    Line l;

    // Begin.
    TiXmlElement *begin=line->FirstChildElement("begin");
    if (begin && begin->FirstChildElement("point"))
    {
        std::string point_s=ToolBox::domToString(begin->FirstChildElement("point"));
        l.setBegin(Point::createFromXml(point_s, error), error);
    }
    else
    {
        setStaticError(error, BadLineBeginPoint);
        return 0;
    }

    // End.
    TiXmlElement *end=line->FirstChildElement("end");
    if (end && end->FirstChildElement("point"))
    {
        std::string point_s=ToolBox::domToString(end->FirstChildElement("point"));
        l.setEnd(Point::createFromXml(point_s, error), error);
    }
    else
    {
        setStaticError(error, BadLineEndPoint);
        return 0;
    }

    return new Line(l);
}

std::vector<Line*> Line::cut(const Point &point) const
{
    std::vector<Line*> lines;

    if (!GeometryTools::equivalentPoints(begin(), point))
    {
        Line *line1=new Line;
        line1->setBegin(begin(), 0);
        line1->setEnd(point, 0);
        lines.push_back(line1);
    }

    if (!GeometryTools::equivalentPoints(end(), point))
    {
        Line *line2=new Line;
        line2->setBegin(point, 0);
        line2->setEnd(end(), 0);
        lines.push_back(line2);
    }

    return lines;
}

Point Line::middlePoint() const
{
    Point p;
    p.setX(0.5*(_begin.x()+_end.x()));
    p.setY(0.5*(_begin.y()+_end.y()));
    return p;
}

bool Line::containsPoint(const Point &point) const
{
    return GeometryTools::containsPoint(this, point);
}

bool Line::containsPath(const Path *path) const
{
    if (path->type()!=LinePath)
        return false;
    const Line *line=(const Line*) path;
    if (!containsPoint(line->begin()))
        return false;
    if (!containsPoint(line->end()))
        return false;
    return true;
}

double Line::distance(const Point &point) const
{
    return GeometryTools::pointDistance(this, point);
}

Point Line::nearMiddlePoint(bool type) const
{
    double nx=end().x()-begin().x();
    double ny=end().y()-begin().y();
    double len=nx*nx+ny*ny;
    nx=nx/sqrt(len);
    ny=ny/sqrt(len);

    double nx_ortho=-ny;
    double ny_ortho=nx;

    Point M=middlePoint();
    double epsilon=1.0e-10;
    double sign=1.0;
    if (type)
        sign=-1.0;
    double dx=nx_ortho*epsilon*sign;
    double dy=ny_ortho*epsilon*sign;

    M.setX(M.x()+dx);
    M.setY(M.y()+dy);
    return M;
}

bool Line::isEquivalentTo(const Path *path) const
{
    if (path->type()!=LinePath)
        return false;
    const Line *line=(const Line*) path;

    if (!containsPath(line))
        return false;
    if (!line->containsPath(this))
        return false;
    return true;
}

double Line::xMin() const
{
    return std::min(_begin.x(), _end.x());
}

double Line::xMax() const
{
    return std::max(_begin.x(), _end.x());
}

double Line::yMin() const
{
    return std::min(_begin.y(), _end.y());
}

double Line::yMax() const
{
    return std::max(_begin.y(), _end.y());
}

void Line::translate(double dx, double dy)
{
    _begin.setX(_begin.x()+dx);
    _begin.setY(_begin.y()+dy);

    _end.setX(_end.x()+dx);
    _end.setY(_end.y()+dy);
}

void Line::setInitialPath(Path *path)
{
    _initialPath=path;
}

Path *Line::initialPath() const
{
    return _initialPath;
}

double Line::angle() const
{
    double dx=_end.x()-_begin.x();
    double dy=_end.y()-_begin.y();

    double angle=atan2(dy, dx)*180.0/M_PI;

    return angle+90.0;
}

double Line::length() const
{
    double dx=_end.x()-_begin.x();
    double dy=_end.y()-_begin.y();

    return sqrt(dx*dx+dy*dy);
}

bool Line::isVertical() const
{
    Point p1;
    p1.setX(begin().x());

    Point p2;
    p2.setX(end().x());

    if (GeometryTools::equivalentPoints(p1, p2))
        return true;
    return false;
}
