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

#include <Arc.h>
#include <tools/ToolBox.h>
#include <algorithm>

using namespace DxfReader;
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

Arc::Arc():
    _radius(0.0),
    _beginAngle(0.0),
    _endAngle(0.0),
    _xCenter(0.0),
    _yCenter(0.0)
{
}

Arc::~Arc()
{
}

Entity::Type Arc::getType() const
{
    return TypeArc;
}

bool Arc::containsPoint(double x, double y, double epsilon) const
{
    x=x-_xCenter;
    y=y-_yCenter;

    double distance=sqrt(x*x+y*y);
    if (fabs(distance-_radius)>epsilon+width())
        return false;

    double theta1=0.0;
    double theta2=endAngle()-beginAngle();
    if (theta2<0.0)
        theta2+=360.0;

    double alpha=beginAngle()*M_PI/180.0;

    double x2=x*cos(alpha)+y*sin(alpha);
    double y2=y*cos(alpha)-x*sin(alpha);

    double alpha_rad=atan2(y2, x2);
    double alpha_deg=alpha_rad*180.0/M_PI;

    if (alpha_deg<0.0)
        return false;
    if (alpha_deg>theta2)
        return false;

    return true;
}

double Arc::minimalX() const
{
    return _xCenter-_radius;
}

double Arc::minimalY() const
{
    return _yCenter-_radius;
}

double Arc::maximalX() const
{
    return _xCenter+_radius;
}

double Arc::maximalY() const
{
    return _yCenter+_radius;
}

void Arc::setCenter(double x, double y)
{
    _xCenter=x;
    _yCenter=y;
}

void Arc::setRadius(double radius)
{
    _radius=radius;
}

void Arc::setBeginAngle(double angle)
{
    _beginAngle=angle;
}

void Arc::setEndAngle(double angle)
{
    _endAngle=angle;
}

double Arc::xCenter() const
{
    return _xCenter;
}

double Arc::yCenter() const
{
    return _yCenter;
}

double Arc::radius() const
{
    return _radius;
}

double Arc::beginAngle() const
{
    return _beginAngle;
}

double Arc::endAngle() const
{
    return _endAngle;
}

std::string Arc::toConducteoElement() const
{
    std::string xml="<arc>";
    
    xml+="<center>";
    xml+="<point>";
    xml+="<x>"+ToolBox::doubleToString(_xCenter)+"</x>";
    xml+="<y>"+ToolBox::doubleToString(_yCenter)+"</y>";
    xml+="</point>";
    xml+="</center>";

    xml+="<horizontal-radius>"+ToolBox::doubleToString(_radius)+"</horizontal-radius>";
    xml+="<vertical-radius>"+ToolBox::doubleToString(_radius)+"</vertical-radius>";
    
    xml+="<begin-point>";
    Point2d begin=ellipsePointAtAngle(_radius, _beginAngle);
    begin.setX(begin.x()+_xCenter);
    begin.setY(begin.y()+_yCenter);
    xml+=begin.toConducteoElement();
    xml+="</begin-point>";

    xml+="<end-point>";
    Point2d end=ellipsePointAtAngle(_radius, _endAngle);
    end.setX(end.x()+_xCenter);
    end.setY(end.y()+_yCenter);
    xml+=end.toConducteoElement();
    xml+="</end-point>";

    xml+="<rotation-angle>0.0</rotation-angle>";

    xml+="</arc>";
    return xml;
}

Point2d Arc::ellipsePointAtAngle(double radius, double theta) const
{
    // Bounds angle between [0 ; 360].
    while (theta<0.0)
        theta+=360.0;
    while (theta>360.0)
        theta-=360.0;

    // Case of angle is 90� or 270�.
    if (theta==90.0)
    {
        Point2d p;
        p.setX(0.0);
        p.setY(radius);
        return p;
    }
    if (theta==90.0)
    {
        Point2d p;
        p.setX(0.0);
        p.setY(-radius);
        return p;
    }

    // Compute x^2 & y^2.
    double ac=radius*radius;
    double bc=radius*radius;
    double tantheta=tan(theta/180.0*M_PI);
    double tanthetac=tantheta*tantheta;

    double xc=1.0/(1.0/ac+tanthetac/bc);
    double yc=(1.0-xc/ac)*bc;

    // Fix numerical zero.
    if (fabs(xc)<1.0e-12)
        xc=0.0;

    if (fabs(yc)<1.0e-12)
        yc=0.0;

    Point2d p;

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
