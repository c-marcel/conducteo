// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2019, Clément MARCEL.
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

#ifndef _TEXT_H
#define _TEXT_H

#include <Entity.h>
#include <string>
#include <vector>

namespace DxfReader
{

class TextStyle;

class Text: public Entity
{

public:

    enum AttachmentPoint
    {
        TopLeft,
        TopCenter,
        TopRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        BottomLeft,
        BottomCenter,
        BottomRight
    };

    Text();
    virtual ~Text();
    virtual Type getType() const;
    virtual bool containsPoint(double x, double y, double epsilon) const;
    virtual double minimalX() const;
    virtual double minimalY() const;
    virtual double maximalX() const;
    virtual double maximalY() const;

    void setText(const std::string &text);
    void parseMText(const std::string &mtext);
    void setAngle(double angle);
    void setAnchorPoint(double X, double Y);
    void setTextHeight(double height);
    void setTextWidth(double width);
    void setStyle(TextStyle *style);
    void setFont(const std::string &font);
    void setBold(bool bold);
    void setItalic(bool italic);
    void setAttachmentPointValue(int value);

    std::string text() const;
    double angle() const;
    double x() const;
    double y() const;
    double textHeight() const;
    double textWidth() const;
    TextStyle *style() const;
    std::string font() const;
    bool bold() const;
    bool italic() const;
    AttachmentPoint attachmentPoint() const;

private:

    std::vector<std::string> extractMTextOptions(const std::string &mtext) const;
    std::string extractMTextText(const std::string &mtext) const;
    std::string replaceUnicode(const std::string &text) const;
    char charFromHexa(std::string &in) const;
    char charFromHexa(const char c) const;
    void applyOptions(const std::vector<std::string> &options);
    void extractColor(const std::string &option);
    void extractHeight(const std::string &option);
    void extractFont(const std::string &option);
    void getFontSize(const std::string &text, int &width, int &height) const;
    std::string replaceSpecialCharacter(const std::string &text, const std::string &in, const std::string &out) const;

    std::string     _text;
    std::string     _font;
    double          _angle;
    double          _height;
    double          _width;
    double          _x;
    double          _y;
    TextStyle      *_style;
    bool            _bold;
    bool            _italic;
    AttachmentPoint _attachmentPoint;

};

}

#endif
