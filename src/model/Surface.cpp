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

#include <model/Surface.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>
#include <geometry/GeometryTools.h>
#include <databases/Environments.h>
#include <errors/ErrorHandler.h>
#include <physics/Environment.h>
#include <geometry/Path.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>
#include <math.h>

using namespace std;

Surface::Surface():
    _boundaryCondition(0),
    _environment(0),
    _flux(0.0),
    _minimalTemperature(0.0),
    _maximalTemperature(0.0),
    _selected(false),
    _usedFixedTemperature(false),
    _fixedTemperature(0.0),
    _minimalRh(0.0),
    _maximalRh(0.0)
{
}

Surface::Surface(const Surface &surface)
{
    _boundaryCondition=surface._boundaryCondition;
    _environment=surface._environment;

    _selected=false;
    _usedFixedTemperature=surface._usedFixedTemperature;
    _fixedTemperature=surface._fixedTemperature;

    for (unsigned int i=0 ; i<surface.size() ; i++)
    {
        Line *line=surface.at(i);
        Line *copy=new Line(*line);
        push_back(copy);
    }
}

Surface::~Surface()
{
    clean();
}

void Surface::clean()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
    resize(0);
}

double Surface::maximalRh() const
{
    return _maximalRh;
}

void Surface::setMaximalRh(double maximalRh)
{
    _maximalRh = maximalRh;
}

double Surface::minimalRh() const
{
    return _minimalRh;
}

void Surface::setMinimalRh(double minimalRh)
{
    _minimalRh = minimalRh;
}

bool Surface::check(ErrorHandler *error)
{
    if (empty())
    {
        setError(error, SurfaceWithoutPath);
        return false;
    }

    for (unsigned int i=0 ; i<size() ; i++)
        if (!at(i)->check(error))
            return false;

    if (_boundaryCondition && !_boundaryCondition->check(error))
        return false;

    if (_environment && !_environment->check(error))
        return false;
    
    return true;
}

void Surface::setBoundaryCondition(BoundaryCondition *bc)
{
    _boundaryCondition=bc;
}

void Surface::setEnvironment(Environment *env)
{
    _environment=env;
}

BoundaryCondition *Surface::boundaryCondition() const
{
    return _boundaryCondition;
}

Environment *Surface::environment() const
{
    if (!_boundaryCondition)
        return 0;
    return _environment;
}

std::string Surface::serializeAsV2Surface() const
{
    if (size()!=1)
        return "";

    if (!_boundaryCondition)
        return "";

    if (_boundaryCondition->id().empty())
        return "";

    std::string xml="<surface>";

    double xmin=std::min(at(0)->begin().x(), at(0)->end().x());
    double ymin=std::min(-at(0)->begin().y(), -at(0)->end().y());

    double xmax=std::max(at(0)->begin().x(), at(0)->end().x());
    double ymax=std::max(-at(0)->begin().y(), -at(0)->end().y());

    xml+="<begin>";
    xml+="<x>"+ToolBox::doubleToString(xmin)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(ymin)+"</y>";
    xml+="</begin>";

    xml+="<end>";
    xml+="<x>"+ToolBox::doubleToString(xmax)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(ymax)+"</y>";
    xml+="</end>";

    xml+="<resistance>"+ToolBox::doubleToString(_boundaryCondition->resistance())+"</resistance>";
    xml+="<temperature>"+ToolBox::doubleToString(_boundaryCondition->temperature())+"</temperature>";
    xml+="<boundary-condition>"+_boundaryCondition->id()+"</boundary-condition>";

    if (_environment)
        xml+="<environment>"+_environment->name()+"</environment>";

    xml+="</surface>";
    return xml;
}

std::string Surface::serialize() const
{
    std::string xml="<surface>";

    xml+="<paths>";
    for (unsigned int i=0 ; i<size() ; i++)
        xml+=at(i)->serialize();
    xml+="</paths>";

    if (_boundaryCondition && !_boundaryCondition->id().empty())
        xml+="<boundary-condition ref=\""+_boundaryCondition->id()+"\" />";

    if (_environment)
        xml+="<environment ref=\""+_environment->name()+"\" />";

    xml+="</surface>";
    return xml;
}

Surface *Surface::createFromXml(const std::string &xml, ErrorHandler *error, std::map<std::string, std::string> &swap)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *surface=doc.RootElement();
    if (!surface)
    {
        setStaticError(error, BadSurfaceNode);
        return 0;
    }

    if (!surface->Value())
    {
        setStaticError(error, BadSurfaceNode);
        return 0;
    }

    std::string root_name=surface->Value();
    if (root_name!="surface")
    {
        setStaticError(error, BadSurfaceNode);
        return 0;
    }

    Surface *s=new Surface;

    // Boundary condition.
    TiXmlElement *bc_n=surface->FirstChildElement("boundary-condition");
    if (bc_n && bc_n->Attribute("ref"))
    {
        std::string bc_ref=bc_n->Attribute("ref");
        BoundaryCondition *m=BoundaryConditions::instance()->bcFromId(bc_ref);

        std::string swap_bc=swap[bc_ref];
        if (!swap_bc.empty())
            m=BoundaryConditions::instance()->bcFromId(swap_bc);

        s->setBoundaryCondition(m);
    }

    // Environment.
    TiXmlElement *environment_n=surface->FirstChildElement("environment");
    if (environment_n && environment_n->Attribute("ref"))
    {
        std::string env_ref=environment_n->Attribute("ref");
        Environment *e=Environments::instance()->environmentFromName(env_ref);
        s->setEnvironment(e);
    }

    // Geometry.
    TiXmlElement *paths=surface->FirstChildElement("paths");
    if (paths)
    {
        TiXmlElement *path=paths->FirstChildElement();
        if (path)
        {
            std::string path_s=ToolBox::domToString(path);
            Line *p=Line::createFromXml(path_s, error);
            if (p)
                s->push_back(p);
        }
    }

    return s;
}

void Surface::setFlux(double flux)
{
    _flux=flux;
}

void Surface::setMinimalTemperature(double temperature)
{
    _minimalTemperature=temperature;
}

void Surface::setMaximalTemperature(double temperature)
{
    _maximalTemperature=temperature;
}

void Surface::setMinimalTemperatureLocation(const Point &location)
{
    _minimalTemperatureLocation=location;
}

void Surface::setMaximalTemperatureLocation(const Point &location)
{
    _maximalTemperatureLocation=location;
}

double Surface::flux() const
{
    return _flux;
}

double Surface::minimalTemperature() const
{
    return _minimalTemperature;
}

double Surface::maximalTemperature() const
{
    return _maximalTemperature;
}

Point Surface::minimalTemperatureLocation() const
{
    return _minimalTemperatureLocation;
}

Point Surface::maximalTemperatureLocation() const
{
    return _maximalTemperatureLocation;
}

void Surface::setSelected(bool selected)
{
    _selected=selected;
}

bool Surface::selected() const
{
    return _selected;
}

bool Surface::containsPoint(const Point &point) const
{
    for (unsigned int i=0 ; i<size() ; i++)
        if (at(i)->containsPoint(point))
            return true;
    return false;
}

double Surface::pointDistance(const Point &point) const
{
    double distance=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        double d=GeometryTools::pointDistance(at(i), point);
        if (d<distance || i==0)
            distance=d;
    }
    return distance;
}

bool Surface::haveCommonPoints(Surface *surface) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        Point b=line->begin();
        Point e=line->end();

        if (surface->containsPoint(b))
            return true;
        if (surface->containsPoint(e))
            return true;
    }
    return false;
}

double Surface::commonArea(Line *line1) const
{
    double area=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line2=at(i);
        std::vector<Point> points=GeometryTools::intersectionPoint(line1, line2);
        if (points.size()!=2)
            continue;

        double dx=points.at(1).x()-points.at(0).x();
        double dy=points.at(1).y()-points.at(0).y();

        area+=sqrt(dx*dx+dy*dy);
    }
    return area;
}

void Surface::setFixedTemperature(double temperature)
{
    _usedFixedTemperature=true;
    _fixedTemperature=temperature;
}

void Surface::unsetFixedTemperature()
{
    _usedFixedTemperature=false;
}

double Surface::temperature() const
{
    if (_usedFixedTemperature)
        return _fixedTemperature;
    if (!_boundaryCondition)
        return 0.0;
    return _boundaryCondition->temperature();
}

double Surface::length() const
{
    double l=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        l+=line->length();
    }
    return l;
}

bool Surface::isVertical() const
{
    if (size()!=1)
        return false;

    double x0=at(0)->begin().x();
    double x1=at(0)->end().x();

    return (fabs(x0-x1)<1.0e-12);
}

bool Surface::isHorizontal() const
{
    if (size()!=1)
        return false;

    double y0=at(0)->begin().y();
    double y1=at(0)->end().y();

    return (fabs(y0-y1)<1.0e-12);
}
