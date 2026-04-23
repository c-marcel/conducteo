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

#ifndef _ENTITY_H
#define _ENTITY_H

#include <string>

namespace DxfReader
{

class Layer;
class Block;

class Entity
{

public:

    enum Type 
    {
        TypeVirtual,
        TypePoint,
        TypeLine,
        TypePolyline,
        TypeArc,
        TypeCircle,
        TypeText,
        TypeInsertData,
        TypeBlock,
        TypePolygon
    };

    enum WidthType
    {
        DefaultWidth,
        ByLayerWidth,
        ByBlockWidth,
        CustomWidth
    };

    enum LineType
    {
        ByLayerLineType,
        ByBlockLineType,
        Continuous,
        Center,
        CenterX2,
        Center2,
        Dashed,
        DashedX2,
        Dashed2,
        Phantom,
        PhantomX2,
        Phantom2,
        DashDot,
        DashDotX2,
        DashDot2,
        Dot,
        DotX2,
        Dot2,
        Divide,
        DivideX2,
        Divide2
    };

    Entity();
    virtual ~Entity();

    void setLayer(Layer *layer);
    Layer *layer() const;

    void setBlock(Block *block);
    Block *block() const;

    void setColor(int red, int green, int blue);
    int redColor() const;
    int greenColor() const;
    int blueColor() const;

    void setWidth(WidthType type, double width=0.0);
    double width() const;

    void setZOrder(int zorder);
    int zOrder() const;

    void setLineType(LineType type);
    LineType lineType() const;

    virtual Type getType() const;
    virtual bool containsPoint(double x, double y, double epsilon) const;
    virtual double minimalX() const;
    virtual double minimalY() const;
    virtual double maximalX() const;
    virtual double maximalY() const;

    static LineType lineTypeFromString(const std::string &type);
    bool visible() const;
    void setVisible(bool visible);

    void setSelected(bool selected);
    bool selected() const;

    virtual std::string toConducteoElement() const;

private:

    int             _redColor;
    int             _greenColor;
    int             _blueColor;
    bool            _visible;
    bool            _selected;
    int             _zOrder;

    WidthType       _widthType;
    double          _width;
    LineType        _lineType;

    Layer       *_layer;
    Block       *_block;

};

}

#endif
