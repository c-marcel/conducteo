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

#include <TextStyle.h>

using namespace DxfReader;

TextStyle::TextStyle():
    _bold(false),
    _italic(false),
    _direction(Horizontal)
{
}

TextStyle::~TextStyle()
{
}

void TextStyle::setName(const std::string &name)
{
    _name=name;
}

void TextStyle::setFontName(const std::string &font)
{
    _font=font;
}

void TextStyle::setBold(bool bold)
{
    _bold=bold;
}

void TextStyle::setItalic(bool italic)
{
    _italic=italic;
}

void TextStyle::setDirection(Direction direction)
{
    _direction=direction;
}

std::string TextStyle::name() const
{
    return _name;
}

std::string TextStyle::fontName() const
{
    return _font;
}

bool TextStyle::bold() const
{
    return _bold;
}

bool TextStyle::italic() const
{
    return _italic;
}

TextStyle::Direction TextStyle::direction() const
{
    return _direction;
}
