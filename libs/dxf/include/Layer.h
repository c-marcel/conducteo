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

#ifndef _LAYER_H
#define _LAYER_H

#include <Entity.h>
#include <vector>

namespace DxfReader
{

class Layer: public std::vector<Entity*>
{

public:

    Layer();
    virtual ~Layer();

    void setName(const std::string &name);
    std::string getName() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setColor(int red, int green, int blue);
    int redColor() const;
    int greenColor() const;
    int blueColor() const;

    void setDefaultWidth(double width);
    double defaultWidth() const;

    void setDefaultLineType(Entity::LineType type);
    Entity::LineType defaultLineType() const;

    double minimalX() const;
    double minimalY() const;
    double maximalX() const;
    double maximalY() const;

    void removeEntityReference(Entity *entity);

private:

    std::string         _name;
    int                 _redColor;
    int                 _greenColor;
    int                 _blueColor;
    bool                _visible;
    double              _defaultWidth;
    Entity::LineType    _defaultLineType;

};

}

#endif
