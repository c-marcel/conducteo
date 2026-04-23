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

#ifndef _ANNOTATION_H
#define _ANNOTATION_H

#include <geometry/Point.h>
#include <string>

class Annotation
{
public:
    enum Type
    {
        AnnotationText,
        AnnotationArrow,
        AnnotationCotation,
        AnnotationAngle
    };

    enum ClickType
    {
        NoClickType = 0,
        Translation = 1,
        Begin       = 2,
        End         = 4,
        Middle      = 8
    };

    enum SelectionPart
    {
        NoSelection = 0,
        Whole       = 1,
        Lines       = 2,
        Text        = 4
    };

    Annotation();
    Annotation(const Annotation &annotation);
    virtual ~Annotation();

    void setType(Type type);
    Type type() const;

    void setText(const std::string &text);
    std::string text() const;

    void setPosition(const Point &point);
    Point position() const;

    void setBegin(const Point &point);
    Point begin() const;

    void setEnd(const Point &point);
    Point end() const;

    std::string serialize() const;
    void importFromXml(const std::string &xml);

    void setId(const std::string &id);
    std::string id() const;

    void setAngleCenter(const Point &center);
    Point angleCenter() const;

    Point firstArrowFirstPoint() const;
    Point firstArrowSecondPoint() const;
    Point secondArrowFirstPoint() const;
    Point secondArrowSecondPoint() const;
    bool validArrow() const;

    bool contains(const Point &point, double precision) const;
    void setTextDimensions(double width, double height);

    void setSelection(SelectionPart selection);
    SelectionPart selection() const;
    void select(double x, double y, double epsilon);

    ClickType clickType(double x, double y, double epsilon) const;

    void updateText();

private:
    void updateArrowPoints();
    bool arrowContainsPoint(const Point &point, double precision) const;
    bool angleContainsPoint(const Point &point, double precision) const;
    bool textContainsPoint(const Point &point, double precision) const;

    ClickType arrowClickType(double x, double y, double epsilon) const;
    ClickType cotationClickType(double x, double y, double epsilon) const;
    ClickType angleClickType(double x, double y, double epsilon) const;

    Type        _type;
    std::string _id;

    Point       _position;
    std::string _text;

    Point       _begin;
    Point       _end;

    bool        _validArrow;
    Point       _firstArrowFirstpoint;
    Point       _firstArrowSecondpoint;
    Point       _secondArrowFirstpoint;
    Point       _secondArrowSecondpoint;

    Point       _angleCenter;

    double      _textWidth;
    double      _textHeight;

    SelectionPart   _selection;
};

#endif
