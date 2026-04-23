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

#include <model/Annotation.h>
#include <geometry/Line.h>
#include <tools/ToolBox.h>
#include <tinyxml.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Annotation::Annotation():
    _type(AnnotationText),
    _id(ToolBox::getNewUuuid()),
    _validArrow(false),
    _selection(NoSelection)
{
}

Annotation::Annotation(const Annotation &annotation)
{
    _type=annotation._type;
    _id=annotation._id;
    _position=annotation._position;
    _text=annotation._text;
    _begin=annotation._begin;
    _end=annotation._end;

    _firstArrowFirstpoint=annotation._firstArrowFirstpoint;
    _firstArrowSecondpoint=annotation._firstArrowSecondpoint;
    _secondArrowFirstpoint=annotation._secondArrowFirstpoint;
    _secondArrowSecondpoint=annotation._secondArrowSecondpoint;
    _validArrow=annotation._validArrow;

    _angleCenter=annotation._angleCenter;

    _selection=NoSelection;
}

Annotation::~Annotation()
{
}

void Annotation::setType(Type type)
{
    _type=type;
}

Annotation::Type Annotation::type() const
{
    return _type;
}

void Annotation::setText(const std::string &text)
{
    _text=text;
}

std::string Annotation::text() const
{
    return _text;
}

void Annotation::setPosition(const Point &point)
{
    _position=point;
}

Point Annotation::position() const
{
    return _position;
}

void Annotation::setBegin(const Point &point)
{
    _begin=point;
    updateArrowPoints();
}

Point Annotation::begin() const
{
    return _begin;
}

void Annotation::setEnd(const Point &point)
{
    _end=point;
     updateArrowPoints();
}

Point Annotation::end() const
{
    return _end;
}

std::string Annotation::serialize() const
{
    std::string xml="<annotation>";

    xml+="<type>";
    if (_type==AnnotationText)
        xml+="text";
    else if (_type==AnnotationArrow)
        xml+="simple-arrow";
    else if (_type==AnnotationCotation)
        xml+="cotation";
    else if (_type==AnnotationAngle)
        xml+="angle-cotation";
    xml+="</type>";

    if (_type==AnnotationText)
    {
        xml+="<position>";
        xml+=_position.serialize();
        xml+="</position>";
        xml+="<text>"+_text+"</text>";
    }

    else if (_type==AnnotationArrow)
    {
        xml+="<begin>";
        xml+=_begin.serialize();
        xml+="</begin>";

        xml+="<end>";
        xml+=_end.serialize();
        xml+="</end>";
    }

    else if (_type==AnnotationCotation)
    {
        xml+="<begin>";
        xml+=_begin.serialize();
        xml+="</begin>";

        xml+="<end>";
        xml+=_end.serialize();
        xml+="</end>";

        xml+="<position>";
        xml+=_position.serialize();
        xml+="</position>";
    }

    else if (_type==AnnotationAngle)
    {
        xml+="<begin>";
        xml+=_begin.serialize();
        xml+="</begin>";

        xml+="<end>";
        xml+=_end.serialize();
        xml+="</end>";

        xml+="<center>";
        xml+=_angleCenter.serialize();
        xml+="</center>";

        xml+="<position>";
        xml+=_position.serialize();
        xml+="</position>";
    }

    xml+="</annotation>";
    return xml;
}

void Annotation::importFromXml(const std::string &xml)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *annotation=doc.RootElement();
    if (!annotation)
        return;

    TiXmlElement *n_type=annotation->FirstChildElement("type");
    if (!n_type)
        return;
    if (!n_type->GetText())
        return;
    
    std::string type=n_type->GetText();

    if (type=="text")
    {
        TiXmlElement *n_position=annotation->FirstChildElement("position");
        TiXmlElement *n_text=annotation->FirstChildElement("text");

        if (!n_position)
            return;
        if (!n_text)
            return;

        TiXmlElement *n_point=n_position->FirstChildElement("point");
        const char *text=n_text->GetText();

        if (!n_point)
            return;
        if (!text)
            return;

        std::string point_s=ToolBox::domToString(n_point);
        _position=Point::createFromXml(point_s, 0);
        _text=text;
        _type=AnnotationText;
    }

    else if (type=="simple-arrow")
    {
        TiXmlElement *n_begin=annotation->FirstChildElement("begin");
        TiXmlElement *n_end=annotation->FirstChildElement("end");

        if (!n_begin)
            return;
        if (!n_end)
            return;

        TiXmlElement *n_begin_point=n_begin->FirstChildElement("point");
        TiXmlElement *n_end_point=n_end->FirstChildElement("point");

        if (!n_begin_point)
            return;
        if (!n_end_point)
            return;

        std::string begin_point_s=ToolBox::domToString(n_begin_point);
        std::string end_point_s=ToolBox::domToString(n_end_point);

        _begin=Point::createFromXml(begin_point_s, 0);
        _end=Point::createFromXml(end_point_s, 0);

        _type=AnnotationArrow;

        updateArrowPoints();
    }

    else if (type=="cotation")
    {
        TiXmlElement *n_begin=annotation->FirstChildElement("begin");
        TiXmlElement *n_end=annotation->FirstChildElement("end");

        if (!n_begin)
            return;
        if (!n_end)
            return;

        TiXmlElement *n_begin_point=n_begin->FirstChildElement("point");
        TiXmlElement *n_end_point=n_end->FirstChildElement("point");

        if (!n_begin_point)
            return;
        if (!n_end_point)
            return;

        std::string begin_point_s=ToolBox::domToString(n_begin_point);
        std::string end_point_s=ToolBox::domToString(n_end_point);

        _begin=Point::createFromXml(begin_point_s, 0);
        _end=Point::createFromXml(end_point_s, 0);

        _type=AnnotationCotation;

        TiXmlElement *n_position=annotation->FirstChildElement("position");

        if (!n_position)
            return;

        TiXmlElement *n_point=n_position->FirstChildElement("point");

        if (!n_point)
            return;

        std::string point_s=ToolBox::domToString(n_point);
        _position=Point::createFromXml(point_s, 0);

        updateArrowPoints();
        updateText();
    }

    else if (type=="angle-cotation")
    {
        TiXmlElement *n_begin=annotation->FirstChildElement("begin");
        TiXmlElement *n_end=annotation->FirstChildElement("end");

        if (!n_begin)
            return;
        if (!n_end)
            return;

        TiXmlElement *n_begin_point=n_begin->FirstChildElement("point");
        TiXmlElement *n_end_point=n_end->FirstChildElement("point");

        if (!n_begin_point)
            return;
        if (!n_end_point)
            return;

        std::string begin_point_s=ToolBox::domToString(n_begin_point);
        std::string end_point_s=ToolBox::domToString(n_end_point);

        _begin=Point::createFromXml(begin_point_s, 0);
        _end=Point::createFromXml(end_point_s, 0);

        _type=AnnotationAngle;

        TiXmlElement *n_position=annotation->FirstChildElement("position");

        if (!n_position)
            return;

        TiXmlElement *n_point=n_position->FirstChildElement("point");

        if (!n_point)
            return;

        std::string point_s=ToolBox::domToString(n_point);
        _position=Point::createFromXml(point_s, 0);

        TiXmlElement *n_angle=annotation->FirstChildElement("center");

        if (!n_angle)
            return;

        TiXmlElement *n_point_center=n_angle->FirstChildElement("point");

        if (!n_point_center)
            return;

        std::string center_point_s=ToolBox::domToString(n_point_center);
        _angleCenter=Point::createFromXml(center_point_s, 0);

        updateArrowPoints();
        updateText();
    }
}

void Annotation::setId(const std::string &id)
{
    _id=id;
}

std::string Annotation::id() const
{
    return _id;
}

Point Annotation::firstArrowFirstPoint() const
{
    return _firstArrowFirstpoint;
}

Point Annotation::firstArrowSecondPoint() const
{
    return _firstArrowSecondpoint;
}

Point Annotation::secondArrowFirstPoint() const
{
    return _secondArrowFirstpoint;
}

Point Annotation::secondArrowSecondPoint() const
{
    return _secondArrowSecondpoint;
}

bool Annotation::validArrow() const
{
    return _validArrow;
}

void Annotation::updateArrowPoints()
{
    double dx=_end.x()-_begin.x();
    double dy=_end.y()-_begin.y();

    double l=sqrt(dx*dx+dy*dy);
    _validArrow=false;
    if (l<1.0e-18)
        return;

    _validArrow=true;
    dx=dx/l;
    dy=dy/l;

    double mx=-dy;
    double my=dx;

    double a=l/20.0;
    double b=0.4*a;

    _firstArrowFirstpoint.setX(_begin.x()+a*dx+b*mx);
    _firstArrowFirstpoint.setY(_begin.y()+a*dy+b*my);

    _firstArrowSecondpoint.setX(_begin.x()+a*dx-b*mx);
    _firstArrowSecondpoint.setY(_begin.y()+a*dy-b*my);

    _secondArrowFirstpoint.setX(_end.x()-a*dx+b*mx);
    _secondArrowFirstpoint.setY(_end.y()-a*dy+b*my);

    _secondArrowSecondpoint.setX(_end.x()-a*dx-b*mx);
    _secondArrowSecondpoint.setY(_end.y()-a*dy-b*my);
}

bool Annotation::arrowContainsPoint(const Point &point, double precision) const
{
    Line line;
    line.setBegin(_begin, 0);
    line.setEnd(_end, 0);
    if (line.distance(point)<precision)
        return true;
    return false;
}

bool Annotation::textContainsPoint(const Point &point, double precision) const
{
    if (point.x()<_position.x()-precision)
        return false;
    if (point.x()>_position.x()+_textWidth+precision)
        return false;

    if (point.y()<_position.y()-precision)
        return false;
    if (point.y()>_position.y()+precision+_textHeight)
        return false;

    return true;
}

Annotation::ClickType Annotation::arrowClickType(double x, double y, double epsilon) const
{
    // Begin.
    double dc = (_begin.x() - x) * (_begin.x() - x) + (_begin.y() - y) * (_begin.y() - y);
    if (dc < epsilon * epsilon)
        return Begin;

    // End.
    dc = (_end.x() - x) * (_end.x() - x) + (_end.y() - y) * (_end.y() - y);
    if (dc < epsilon * epsilon)
        return End;

    // Default.
    return Translation;
}

Annotation::ClickType Annotation::cotationClickType(double x, double y, double epsilon) const
{
    // Begin.
    double dc = (_begin.x() - x) * (_begin.x() - x) + (_begin.y() - y) * (_begin.y() - y);
    if (dc < epsilon * epsilon)
        return Begin;

    // End.
    dc = (_end.x() - x) * (_end.x() - x) + (_end.y() - y) * (_end.y() - y);
    if (dc < epsilon * epsilon)
        return End;

    // Default.
    return Translation;
}

Annotation::ClickType Annotation::angleClickType(double x, double y, double epsilon) const
{
    // Begin.
    double dc = (_begin.x() - x) * (_begin.x() - x) + (_begin.y() - y) * (_begin.y() - y);
    if (dc < epsilon * epsilon)
        return Begin;

    // End.
    dc = (_end.x() - x) * (_end.x() - x) + (_end.y() - y) * (_end.y() - y);
    if (dc < epsilon * epsilon)
        return End;

    // Middle.
    dc = (_angleCenter.x() - x) * (_angleCenter.x() - x) + (_angleCenter.y() - y) * (_angleCenter.y() - y);
    if (dc < epsilon * epsilon)
        return Middle;

    // Default.
    return Translation;
}

bool Annotation::angleContainsPoint(const Point &point, double precision) const
{
    Line line;
    line.setBegin(_angleCenter, 0);
    line.setEnd(_begin, 0);
    if (line.distance(point)<precision)
        return true;

    line.setBegin(_angleCenter, 0);
    line.setEnd(_end, 0);
    if (line.distance(point)<precision)
        return true;
    return false;
}

bool Annotation::contains(const Point &point, double precision) const
{
    // Arrow
    if (_type==AnnotationArrow)
        return arrowContainsPoint(point, precision);

    // Text.
    else if (_type==AnnotationText)
        return textContainsPoint(point, precision);

    // Cotation.
    else if (_type==AnnotationCotation)
    {
        if (arrowContainsPoint(point, precision))
            return true;
        if (textContainsPoint(point, precision))
            return true;
        return false;
    }

    // Angle.
    else if (_type==AnnotationAngle)
    {
        if (angleContainsPoint(point, precision))
            return true;
        if (textContainsPoint(point, precision))
            return true;
        return false;
    }
   return false;
}

void Annotation::setTextDimensions(double width, double height)
{
    _textWidth=width;
    _textHeight=height;
}

void Annotation::setSelection(SelectionPart selection)
{
    _selection=selection;
}

Annotation::SelectionPart Annotation::selection() const
{
    return _selection;
}

void Annotation::select(double x, double y, double epsilon)
{
    // Text & arrow.
    if (_type == AnnotationText || _type == AnnotationArrow)
        _selection = Whole;

    // Cotation.
    else if (_type == AnnotationCotation)
    {
        Point p;
        p.setX(x);
        p.setY(y);

        if (arrowContainsPoint(p, epsilon))
            _selection = (SelectionPart) (_selection | Lines);

        else if (textContainsPoint(p, epsilon))
            _selection = (SelectionPart) (_selection | Text);
    }

    // Angle.
    else if (_type == AnnotationAngle)
    {
        Point p;
        p.setX(x);
        p.setY(y);

        if (angleContainsPoint(p, epsilon))
            _selection = (SelectionPart) (_selection | Lines);

        else if (textContainsPoint(p, epsilon))
            _selection = (SelectionPart) (_selection | Text);
    }
}

Annotation::ClickType Annotation::clickType(double x, double y, double epsilon) const
{
    // Arrow
    if (_type==AnnotationArrow)
        return arrowClickType(x, y, epsilon);

    // Text.
    else if (_type==AnnotationText)
        return Translation;

    // Cotation.
    else if (_type==AnnotationCotation)
        return cotationClickType(x, y, epsilon);

    // Angle.
    else if (_type==AnnotationAngle)
        return angleClickType(x, y, epsilon);

    // Unknown.
    return NoClickType;
}

void Annotation::updateText()
{
    if (_type == AnnotationText || _type == AnnotationArrow)
        return;

    // Cotation.
    if (_type == Annotation::AnnotationCotation)
    {
        double x0 = begin().x();
        double y0 = begin().y();

        double x1 = end().x();
        double y1 = end().y();

        double dx=x1-x0;
        double dy=y1-y0;
        double l=sqrt(dx*dx+dy*dy);

        if (l<1.0e-8)
            return setText("");

        std::string dist = ToolBox::doubleToString(l, 3) + " m";
        if (l<1.0)
            dist = ToolBox::doubleToString(l * 1000.0, 0) + " mm";

        dist = ToolBox::useDecimalSeparator(dist);

        setText(dist);
    }

    // Angle.
    else if (_type == Annotation::AnnotationAngle)
    {
        Point center = angleCenter();
        Point begin  = Annotation::begin();
        Point end    = Annotation::end();

        double y0=begin.y()-center.y();
        double x0=begin.x()-center.x();
        double theta0=atan2(y0, x0)*180.0/M_PI;

        double y1=end.y()-center.y();
        double x1=end.x()-center.x();
        double theta1=atan2(y1, x1)*180.0/M_PI;

        double theta=fabs(theta0-theta1);
        if (theta>180.0)
            theta=360-theta;

        std::string text = ToolBox::doubleToString(theta, 3) + "°";
        text = ToolBox::useDecimalSeparator(text);

        setText(text);
    }
}

void Annotation::setAngleCenter(const Point &center)
{
    _angleCenter=center;
}

Point Annotation::angleCenter() const
{
    return _angleCenter;
}
