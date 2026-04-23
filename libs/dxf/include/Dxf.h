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

#ifndef _DXF_H
#define _DXF_H

#include <libdxf/dl_creationadapter.h>
#include <vector>
#include <string>

namespace DxfReader
{

class TextStyle;
class Entity;
class Layer;
class Block;
class Text;

class Dxf: public std::vector<Layer*>, 
           public DL_CreationAdapter
{

public:

    Dxf();
    virtual ~Dxf();

    void loadFromFile(const std::string &filename);
    void loadFromStream(const std::string &data);

    std::string data() const;

    double minimalX() const;
    double minimalY() const;
    double maximalX() const;
    double maximalY() const;

    bool visible() const;
    void setVisible(bool visible);

private:

    Dxf(const Dxf &);
    Dxf& operator=(const Dxf &);

    virtual void addLayer(const DL_LayerData &data);
    virtual void addPoint(const DL_PointData &data);
    virtual void addLine(const DL_LineData &data);
    virtual void addBlock(const DL_BlockData &data);
    virtual void endBlock();
    virtual void addInsert(const DL_InsertData &data);
    virtual void addArc(const DL_ArcData &data);
    virtual void addCircle(const DL_CircleData &data);
    virtual void addPolyline(const DL_PolylineData &data);
    virtual void addVertex(const DL_VertexData &data);
    virtual void endEntity();
    virtual void addTextStyle(const DL_StyleData &data);
    virtual void addText(const DL_TextData &data);
    virtual void addMText(const DL_MTextData &data);
    virtual void addMTextChunk(const std::string &data);
    virtual void addDimLinear(const DL_DimensionData &dimensions, const DL_DimLinearData &data);
    virtual void addSolid(const DL_SolidData &data);
    virtual void setVariableInt(const std::string&, int, int);

    void processEntity(Entity *entity, bool forceToLayer=false);
    void addEntityToCurrentLayer(Entity *entity);
    void addEntityToCurrentBlock(Entity *entity);
    void defineEntityColor(Entity *entity);
    void defineEntityLine(Entity *entity);
    Block *getBlockByName(const std::string &name) const;
    void associateTextStyle(Text *text, const std::string &style_name);
    void finalizeInsertDataAndBlocks();

    std::string     _dxfStream;

    Block                  *_currentBlock;
    std::vector<Block*>     _blocks;
    Entity                 *_currentEntity;
    std::vector<TextStyle*> _textStyles;
    std::string             _currentText;
    bool                    _visible;
    double                  _unitConversion;
    int                     _zOrder;

};

}

#endif
