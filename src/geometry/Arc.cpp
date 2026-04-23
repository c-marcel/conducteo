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

#include <geometry/Arc.h>
#include <geometry/transformations/Transformations.h>
#include <geometry/transformations/Translation.h>
#include <geometry/transformations/Rotation.h>
#include <geometry/GeometryTools.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <geometry/Line.h>
#include <tinyxml.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Arc::Arc():
    _horizontalRadius(0.0),
    _verticalRadius(0.0),
    _rotationAngle(0.0)
{
}

Arc::~Arc()
{
    clean();
}

Arc::Arc(const Arc &arc)
{
    _center=arc._center;
    _horizontalRadius=arc._horizontalRadius;
    _verticalRadius=arc._verticalRadius;
    _beginPoint=arc._beginPoint;
    _endPoint=arc._endPoint;
    _rotationAngle=arc._rotationAngle;

    for (unsigned int i=0 ; i<arc.size() ; i++)
    {
        Line *line=arc.at(i);
        Line *copy=new Line(*line);
        push_back(copy);
    }
}

bool Arc::check(ErrorHandler *error)
{
    if (_horizontalRadius<=0.0)
    {
        setError(error, BadArcHorizontalRadius);
        return false;
    }

    if (_verticalRadius<=0.0)
    {
        setError(error, BadArcVerticalRadius);
        return false;
    }

    if (!containsPoint(_beginPoint))
    {
        setError(error, BadArcBeginPoint);
        return false;
    }

    if (!containsPoint(_endPoint))
    {
        setError(error, BadArcEndPoint);
        return false;
    }

    return true;
}

void Arc::recreateLines()
{
    clean();

    double ratio1=_horizontalRadius/_verticalRadius;
    double ratio2=_verticalRadius/_horizontalRadius;
    double theta=10.0*std::min(ratio1, ratio2);
    theta=std::max(theta, 1.0);

    double theta1=angleAtPoint(_beginPoint);
    double theta2=angleAtPoint(_endPoint);
    while (theta2<=theta1)
        theta2+=360.0;
    double angle=theta2-theta1;
    int n=int((angle)/theta)+1;

    Rotation rotation;
    rotation.setAngle(_rotationAngle);

    Translation translation;
    translation.setHorizontalDistance(_center.x());
    translation.setVerticalDistance(_center.y());

    Point p1=begin();

    for (int i=0 ; i<n ; i++)
    {
        double t=theta1+theta*(i+1);

        Point p2=GeometryTools::ellipsePointAtAngle(_horizontalRadius, _verticalRadius, t);

        p2=rotation.apply(p2);
        p2=translation.apply(p2);

        if (t>theta1+angle)
        {
            Line *line=new Line;
            line->setBegin(p1, 0);
            line->setEnd(end(), 0);
            push_back(line);
            break;
        }

        Line *line=new Line;
        line->setBegin(p1, 0);
        line->setEnd(p2, 0);
        push_back(line);

        p1=p2;
    }
}

void Arc::clean()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
    resize(0);
}

Path::Type Arc::type() const
{
    return ArcPath;
}

Point Arc::begin() const
{
    return _beginPoint;
}

Point Arc::end() const
{
    return _endPoint;
}

double Arc::angleAtPoint(const Point &point) const
{
    // Translation to ellipse center.
    Translation translation;
    translation.setHorizontalDistance(-_center.x());
    translation.setVerticalDistance(-_center.y());
    Point p=translation.apply(point);

    // Rotation from ellipse rotation angle.
    Rotation rotation;
    rotation.setAngle(-_rotationAngle);
    p=rotation.apply(p);

    // Compute angle.
    return atan2(p.y(), p.x())*180.0/M_PI;
}

bool Arc::containsPoint(const Point &point) const
{
    return GeometryTools::containsPoint(this, point);
}

bool Arc::containsPath(const Path *path) const
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->containsPath(path))
            return true;
    }
    return false;
}

double Arc::distance(const Point &point) const
{
    double d=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Line *line=at(i);
        double dist=GeometryTools::pointDistance(line, point);
        if (dist<d || i==0)
            d=dist;
    }
    return d;
}

void Arc::setRotationAngle(double angle)
{
    _rotationAngle=angle;
}

void Arc::setCenter(const Point &center)
{
    _center=center;
}

void Arc::setHorizontalRadius(double radius, ErrorHandler *error)
{
    if (radius<=0.0)
    {
        setError(error, BadRadius);
        return;
    }
    _horizontalRadius=radius;
}

void Arc::setVerticalRadius(double radius, ErrorHandler *error)
{
    if (radius<=0.0)
    {
        setError(error, BadRadius);
        return;
    }
    _verticalRadius=radius;
}

double Arc::rotationAngle() const
{
    return _rotationAngle;
}

Point Arc::center() const
{
    return _center;
}

double Arc::horizontalRadius() const
{
    return _horizontalRadius;
}

double Arc::verticalRadius() const
{
    return _verticalRadius;
}

std::string Arc::serialize() const
{
    std::string xml="<arc>";
    
    xml+="<center>";
    xml+=_center.serialize();
    xml+="</center>";

    xml+="<horizontal-radius>"+ToolBox::doubleToString(_horizontalRadius)+"</horizontal-radius>";
    xml+="<vertical-radius>"+ToolBox::doubleToString(_verticalRadius)+"</vertical-radius>";
    
    xml+="<begin-point>";
    xml+=_beginPoint.serialize();
    xml+="</begin-point>";

    xml+="<end-point>";
    xml+=_endPoint.serialize();
    xml+="</end-point>";

    xml+="<rotation-angle>"+ToolBox::doubleToString(_rotationAngle)+"</rotation-angle>";

    xml+="</arc>";
    return xml;
}

Arc *Arc::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *arc=doc.RootElement();
    if (!arc)
    {
        setStaticError(error, BadArcNode);
        return 0;
    }

    if (!arc->Value())
    {
        setStaticError(error, BadArcNode);
        return 0;
    }

    std::string root_name=arc->Value();
    if (root_name!="arc")
    {
        setStaticError(error, BadArcNode);
        return 0;
    }

    Arc a;

    // Center.
    TiXmlElement *center=arc->FirstChildElement("center");
    if (center && center->FirstChildElement("point"))
    {
        std::string point_s=ToolBox::domToString(center->FirstChildElement("point"));
        a.setCenter(Point::createFromXml(point_s, error));
    }
    else
    {
        setStaticError(error, BadArcCenter);
        return 0;
    }

    // Radius & angle.
    TiXmlElement *h_radius=arc->FirstChildElement("horizontal-radius");
    if (h_radius && h_radius->GetText())
        a.setHorizontalRadius(ToolBox::convertStringToDouble(h_radius->GetText()), error);
    else
    {
        setStaticError(error, BadArcHorizontalRadius);
        return 0;
    }

    TiXmlElement *v_radius=arc->FirstChildElement("vertical-radius");
    if (v_radius && v_radius->GetText())
        a.setVerticalRadius(ToolBox::convertStringToDouble(v_radius->GetText()), error);
    else
    {
        setStaticError(error, BadArcVerticalRadius);
        return 0;
    }

    TiXmlElement *beginPoint=arc->FirstChildElement("begin-point");
    if (beginPoint && beginPoint->FirstChildElement("point"))
    {
        std::string point_s=ToolBox::domToString(beginPoint->FirstChildElement("point"));
        a.setBegin(Point::createFromXml(point_s, error), error);
    }
    else
    {
        setStaticError(error, BadArcBeginPoint);
        return 0;
    }

    TiXmlElement *endPoint=arc->FirstChildElement("end-point");
    if (endPoint && endPoint->FirstChildElement("point"))
    {
        std::string point_s=ToolBox::domToString(endPoint->FirstChildElement("point"));
        a.setEnd(Point::createFromXml(point_s, error), error);
    }
    else
    {
        setStaticError(error, BadArcEndPoint);
        return 0;
    }

    TiXmlElement *rotation_angle=arc->FirstChildElement("rotation-angle");
    if (rotation_angle && rotation_angle->GetText())
        a.setRotationAngle(ToolBox::convertStringToDouble(rotation_angle->GetText()));

    Arc *new_arc=new Arc(a);
    new_arc->recreateLines();
    return new_arc;
}

double Arc::xMin() const
{
    double v=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->xMin()<v || i==0)
            v=at(i)->xMin();
    }
    return v;
}

double Arc::xMax() const
{
    double v=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->xMax()>v || i==0)
            v=at(i)->xMax();
    }
    return v;
}

double Arc::yMin() const
{
    double v=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->yMin()<v || i==0)
            v=at(i)->yMin();
    }
    return v;
}

double Arc::yMax() const
{
    double v=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        if (at(i)->yMax()>v || i==0)
            v=at(i)->yMax();
    }
    return v;
}

void Arc::setAngle(double begin, double angle, ErrorHandler *error, bool direct)
{
    // For tests only!
    Transformations transformations;

    Rotation *rotation=new Rotation;
    rotation->setAngle(rotationAngle());
    transformations.append(rotation);

    Translation *translation=new Translation;
    translation->setHorizontalDistance(center().x());
    translation->setVerticalDistance(center().y());
    transformations.append(translation);

    double a=begin;
    if (!direct)
        a=begin+angle;

    _beginPoint=GeometryTools::ellipsePointAtAngle(_horizontalRadius, _verticalRadius, a);
    _beginPoint=transformations.apply(_beginPoint);

    a=begin+angle;
    if (!direct)
        a=begin;

    _endPoint=GeometryTools::ellipsePointAtAngle(_horizontalRadius, _verticalRadius, a);
    _endPoint=transformations.apply(_endPoint);
}

void Arc::setBegin(const Point &point, ErrorHandler *error)
{
    if (!isPointOnEllipse(point) && error)
    {
        setError(error, BadArcBeginPoint);
        return;
    }
    _beginPoint=point;
}

bool Arc::isPointOnEllipse(const Point &point) const
{
    double x=point.x();
    double y=point.y();

    double xc=_center.x();
    double yc=_center.y();

    double a=_horizontalRadius;
    double b=_verticalRadius;

    double ac=a*a;
    double bc=b*b;

    double delta=_rotationAngle/180.0*M_PI;
    double cosd=cos(delta);
    double sind=sin(delta);

    double eq2a=((x-xc)*cosd+(y-yc)*sind)/a;
    eq2a=eq2a*eq2a;

    double eq2b=((y-yc)*cosd-(x-xc)*sind)/b;
    eq2b=eq2b*eq2b;

    double eq2=eq2a+eq2b-1.0;
    if (fabs(eq2)<=1.0e-9)
        return true;

    eq2a=((x-xc)*cosd+(y-yc)*sind)*b;
    eq2a=eq2a*eq2a;

    eq2b=((y-yc)*cosd-(x-xc)*sind)*a;
    eq2b=eq2b*eq2b;

    eq2=eq2a+eq2b-ac*ac*bc*bc;
    if (fabs(eq2)<=1.0e-9)
        return true;

    return false;
}

void Arc::setEnd(const Point &point, ErrorHandler *error)
{
    if (!isPointOnEllipse(point) && error)
    {
        setError(error, BadArcEndPoint);
        return;
    }
    _endPoint=point;
}

void Arc::translate(double dx, double dy)
{
    _center.setX(_center.x()+dx);
    _center.setY(_center.y()+dy);

    _beginPoint.setX(_beginPoint.x()+dx);
    _beginPoint.setY(_beginPoint.y()+dy);

    _endPoint.setX(_endPoint.x()+dx);
    _endPoint.setY(_endPoint.y()+dy);

    recreateLines();
}
