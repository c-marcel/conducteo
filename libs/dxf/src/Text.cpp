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

#include <Text.h>
#include <libdxf/dl_codes.h>
#include <algorithm>
#include <sstream>

#ifdef WIN32
#include <Windows.h>
#endif

using namespace DxfReader;

Text::Text():
    _angle(0.0),
    _height(0.0),
    _x(0.0),
    _y(0.0),
    _style(0),
    _width(0.0),
    _bold(false),
    _italic(false),
    _attachmentPoint(TopLeft)
{
}

Text::~Text()
{
}

Entity::Type Text::getType() const
{
    return TypeText;
} 

bool Text::containsPoint(double x, double y, double epsilon) const
{
    if (_angle!=0.0)
        return false;

    if (x<minimalX())
        return false;
    if (x>maximalX())
        return false;
    if (y<minimalY())
        return false;
    if (y>maximalY())
        return false;
    return true;
}

void Text::getFontSize(const std::string &text, int &width, int &height) const
{
#ifdef WIN32
    HDC dc=GetDC(0);
    RECT r={0, 0, 0, 0};
    DrawTextA(dc, text.c_str(), text.length(), &r, DT_CALCRECT);
    width=int((r.right-r.left)*1.5);
    height=int((r.bottom-r.top)*1.5);
    ReleaseDC(0, dc);

    if (_bold)
        width=int(width*1.1);
#else
    width=0;
    height=0;
#endif
}

double Text::minimalX() const
{
    int width;
    int height;
    getFontSize(_text, width, height);
    width=int(_height/height*width*1.5);

    int dx=0;
    if (_attachmentPoint==TopCenter || _attachmentPoint==MiddleCenter || _attachmentPoint==BottomCenter)
        dx=int(0.5*width);
    if (_attachmentPoint==TopRight || _attachmentPoint==MiddleRight || _attachmentPoint==BottomRight)
        dx=width;

    return _x-dx;
}

double Text::minimalY() const
{
    int dy=0;
    if (_attachmentPoint==TopLeft || _attachmentPoint==TopCenter || _attachmentPoint==TopRight)
        dy=int(0.5*_height);
    if (_attachmentPoint==MiddleLeft || _attachmentPoint==MiddleCenter || _attachmentPoint==MiddleRight)
        dy=int(_height);

    return _y-dy;
}

double Text::maximalX() const
{
    int width;
    int height;
    getFontSize(_text, width, height);
    width=int(_height/height*width*1.5);

    int dx=0;
    if (_attachmentPoint==TopCenter || _attachmentPoint==MiddleCenter || _attachmentPoint==BottomCenter)
        dx=int(0.5*width);
    if (_attachmentPoint==TopLeft || _attachmentPoint==MiddleLeft || _attachmentPoint==BottomLeft)
        dx=width;

    return _x+dx;
}

double Text::maximalY() const
{
    int dy=0;
    if (_attachmentPoint==TopLeft || _attachmentPoint==TopCenter || _attachmentPoint==TopRight)
        dy=int(_height);
    if (_attachmentPoint==MiddleLeft || _attachmentPoint==MiddleCenter || _attachmentPoint==MiddleRight)
        dy=int(0.5*_height);

    return _y+dy;
}

void Text::setText(const std::string &text)
{
    _text=text;
    _text=replaceSpecialCharacter(_text, "%%c", "\u00F8");
    _text=replaceSpecialCharacter(_text, "%%C", "\u00D8");
    _text=replaceSpecialCharacter(_text, "%%p", "\u00B1");
    _text=replaceSpecialCharacter(_text, "%%P", "\u00B1");
    _text=replaceSpecialCharacter(_text, "%%%", "\u0025");
    _text=replaceSpecialCharacter(_text, "%%d", "\u00B0");
    _text=replaceSpecialCharacter(_text, "%%D", "\u00B0");
}

void Text::parseMText(const std::string &mtext)
{
    std::string text=extractMTextText(mtext);
    text=replaceUnicode(text);
    setText(text);

    std::vector<std::string> options=extractMTextOptions(mtext);
    applyOptions(options);
}

std::string Text::replaceUnicode(const std::string &text) const
{
    std::string out=text;
    size_t position=out.find("\\U+");
    while (position!=std::string::npos)
    {
        std::string code=out.substr(position+3, 4);
        char c=charFromHexa(code);
        std::string rep;
        rep+=c;
        out.replace(position, 7, rep);
        position=out.find("\\U+");
    }
    return out;
}

std::string Text::replaceSpecialCharacter(const std::string &text, const std::string &in, const std::string &out) const
{
    std::string o=text;
    size_t position=o.find(in);
    while (position!=std::string::npos)
    {
        o.replace(position, in.length(), out);
        position=o.find(in);
    }
    return o;
}

char Text::charFromHexa(std::string &in) const
{
    std::transform(in.begin(), in.end(), in.begin(), ::tolower);

    if (in.length()!=4)
        return ' ';
    char c=charFromHexa(in.at(3));
    c+=16*charFromHexa(in.at(2));
    c+=16*16*charFromHexa(in.at(1));
    c+=16*16*16*charFromHexa(in.at(0));
    return c;
}

char Text::charFromHexa(const char c) const
{
    if (c>=48 && c<=57)
        return c-48;
    return c-97+10;
}

void Text::setAngle(double angle)
{
    _angle=angle;
}

void Text::setAnchorPoint(double x, double y)
{
    _x=x;
    _y=y;
}

void Text::setTextHeight(double height)
{
    _height=height;
}

void Text::setTextWidth(double width)
{
    _width=width;
}

void Text::setStyle(TextStyle *style)
{
    _style=style;
}

void Text::setFont(const std::string &font)
{
    _font=font;
}

void Text::setBold(bool bold)
{
    _bold=bold;
}

void Text::setItalic(bool italic)
{
    _italic=italic;
}

void Text::setAttachmentPointValue(int value)
{
    if (value==2)
        _attachmentPoint=TopCenter;
    else if (value==3)
        _attachmentPoint=TopRight;
    else if (value==4)
        _attachmentPoint=MiddleLeft;
    else if (value==5)
        _attachmentPoint=MiddleCenter;
    else if (value==6)
        _attachmentPoint=MiddleRight;
    else if (value==7)
        _attachmentPoint=BottomLeft;
    else if (value==8)
        _attachmentPoint=BottomCenter;
    else if (value==9)
        _attachmentPoint=BottomRight;
    else
        _attachmentPoint=TopLeft;
}

std::string Text::text() const
{
    return _text;
}

double Text::angle() const
{
    return _angle;
}

double Text::x() const
{
    return _x;
}

double Text::y() const
{
    return _y;
}

double Text::textHeight() const
{
    return _height;
}

TextStyle *Text::style() const
{
    return _style;
}

std::string Text::font() const
{
    return _font;
}

bool Text::bold() const
{
    return _bold;
}

bool Text::italic() const
{
    return _italic;
}

double Text::textWidth() const
{
    return _width;
}

Text::AttachmentPoint Text::attachmentPoint() const
{
    return _attachmentPoint;
}

std::vector<std::string> Text::extractMTextOptions(const std::string &mtext) const
{
    std::string in=mtext;
    std::vector<std::string> options;

    size_t begin=in.find("\\");

    while (begin!=std::string::npos)
    {
        if (begin+1>=in.length())
            return options;

        if (in.at(begin+1)=='\\')
            return options;

        size_t end=in.find(";", begin);
        if (end==std::string::npos)
            return options;

        std::string extract=in.substr(begin, end-begin);
        options.push_back(extract);

        in=in.substr(end, in.length()-end);
        begin=in.find("\\");
    }

    return options;
}

std::string Text::extractMTextText(const std::string &mtext) const
{
    size_t begin=mtext.find("\\");

    if (begin==std::string::npos)
        return mtext;

    if (begin+1>=mtext.length())
        return mtext;

    if (mtext.at(begin+1)=='\\')
        return mtext;

    size_t end=mtext.find(";", begin);
    if (end==std::string::npos)
        return mtext;   //< Erreur.

    std::string extract=mtext.substr(end+1, mtext.length()-end-1);
    return extractMTextText(extract);
}

void Text::applyOptions(const std::vector<std::string> &options)
{
    for (unsigned int i=0 ; i<options.size() ; i++)
    {
        extractColor(options.at(i));
        extractHeight(options.at(i));
        extractFont(options.at(i));
    }
}

void Text::extractColor(const std::string &option)
{
    size_t begin=option.find("\\C");
    if (begin==std::string::npos)
        return;

    std::string color=option.substr(begin+2, option.length()-begin-2);
    std::istringstream buffer(color);
    int c;
    buffer >> c;
    int red=int(dxfColors[c][0]*255.0);
    int green=int(dxfColors[c][1]*255.0);
    int blue=int(dxfColors[c][2]*255.0);
    setColor(red, green, blue);
}

void Text::extractHeight(const std::string &option)
{
    size_t begin=option.find("\\H");
    if (begin==std::string::npos)
        return;

    std::string height=option.substr(begin+2, option.length()-begin-2);
    std::istringstream buffer(height);
    buffer >> _height;
}

void Text::extractFont(const std::string &option)
{
    size_t begin=option.find("\\f");
    if (begin==std::string::npos)
        return;

    size_t end=option.find("|", begin);
    if (end==std::string::npos)
        end=option.length();

    _font=option.substr(begin+2, end-begin-2);

    if (option.find("b1")!=std::string::npos)
        _bold=true;
    else if (option.find("b0")!=std::string::npos)
        _bold=false;

    if (option.find("i1")!=std::string::npos)
        _italic=true;
    else if (option.find("i0")!=std::string::npos)
        _italic=false;
}
