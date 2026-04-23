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

#ifndef _DXF_VIEWER_H
#define _DXF_VIEWER_H

#include <Entity.h>
#include <vector>
#include <QPoint>

class QMouseEvent;
class QPainter;
class Zoom;

namespace DxfReader
{
    class InsertData;
    class Vertex;
    class Layer;
    class Dxf;
};

class DxfViewer
{

public:

    DxfViewer(Zoom *zoom);
    virtual ~DxfViewer();

protected:

    void drawDxf(QPainter &painter, DxfReader::Dxf *document);
    void drawEntities(bool draw_selected, QPainter &painter, std::vector<DxfReader::Entity*> *entities, DxfReader::InsertData *insert_data=0, bool forceDrawing = false);
    void drawPoint(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);
    void drawLine(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);
    void drawBlock(bool draw_selected, QPainter &painter, DxfReader::Entity *entity);
    void drawArc(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);
    void drawCircle(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);
    void drawPolyline(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);
    void drawPolygon(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);
    void drawPolylinePoint(QPainter &painter, const DxfReader::Vertex &v1, const DxfReader::Vertex &v2, DxfReader::InsertData *insert_data=0);
    void drawText(QPainter &painter, DxfReader::Entity *entity, DxfReader::InsertData *insert_data=0);

    void setDrawingProperties(QPainter &painter, DxfReader::Entity *entity, bool forceSelected);
    double applyInsertionTransformsX(double x, DxfReader::InsertData *insert_data) const;
    double applyInsertionTransformsY(double y, DxfReader::InsertData *insert_data) const;
    void prepareInsertion(QPainter &painter, DxfReader::InsertData *insert_data);
    void endInsertion(QPainter &painter, DxfReader::InsertData *insert_data);
    Qt::PenStyle penStyleFromLineType(DxfReader::Entity::LineType type) const;

    Zoom    *_viewZoom;
    
};

#endif
