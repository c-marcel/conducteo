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

#ifndef _TEXT_STYLE_H
#define _TEXT_STYLE_H

#include <string>

namespace DxfReader
{

class TextStyle
{

public:

    enum Direction
    {
        Horizontal,
        Vertical
    };

    TextStyle();
    virtual ~TextStyle();

    void setName(const std::string &name);
    void setFontName(const std::string &font);
    void setBold(bool bold);
    void setItalic(bool italic);
    void setDirection(Direction direction);

    std::string name() const;
    std::string fontName() const;
    bool bold() const;
    bool italic() const;
    Direction direction() const;

private:

    std::string     _name;
    std::string     _font;
    bool            _bold;
    bool            _italic;
    Direction       _direction;

};

}

#endif
