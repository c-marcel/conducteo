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

#include <DxfViewer.h>
#include <QFontMetrics>
#include <InsertData.h>
#include <QMouseEvent>
#include <Polyline.h>
#include <Polygon.h>
#include <Point2d.h>
#include <QPainter>
#include <Circle.h>
#include <Layer.h>
#include <Block.h>
#include <Text.h>
#include <Line.h>
#include <Zoom.h>
#include <Arc.h>
#include <Dxf.h>
#include <cmath>

using namespace DxfReader;

DxfViewer::DxfViewer(Zoom *zoom):
    _viewZoom(zoom)
{
}

DxfViewer::~DxfViewer()
{
}

void DxfViewer::drawDxf(QPainter &painter, DxfReader::Dxf *document)
{
    if (!document->visible())
        return;

    for (unsigned int i = 0; i < document->size(); i++)
    {
        Layer *layer=document->at(i);
        if (!layer->isVisible())
            continue;

        drawEntities(false, painter, layer);
    }

    for (unsigned int i = 0; i < document->size(); i++)
    {
        Layer *layer=document->at(i);
        if (!layer->isVisible())
            continue;

        drawEntities(true, painter, layer);
    }
}

void DxfViewer::drawEntities(bool draw_selected, QPainter &painter, std::vector<Entity*> *entities, InsertData *insert_data, bool forceDrawing)
{
    prepareInsertion(painter, insert_data);

    for (unsigned int i=0 ; i<entities->size() ; i++)
    {
        Entity *entity=entities->at(i);
        Entity::Type type=entity->getType();

        if (!entity->visible())
            continue;

        if ((entity->selected()!=draw_selected) && (!forceDrawing))
            continue;

        setDrawingProperties(painter, entity, forceDrawing);

        if (type==Entity::TypePoint)
            drawPoint(painter, entity, insert_data);
        else if (type==Entity::TypeLine)
            drawLine(painter, entity, insert_data);
        else if (type==Entity::TypeInsertData)
            drawBlock(draw_selected, painter, entity);
        else if (type==Entity::TypeArc)
            drawArc(painter, entity, insert_data);
        else if (type==Entity::TypeCircle)
            drawCircle(painter, entity, insert_data);
        else if (type==Entity::TypePolyline)
            drawPolyline(painter, entity, insert_data);
        else if (type==Entity::TypeText)
            drawText(painter, entity, insert_data);
        else if (type==Entity::TypePolygon)
            drawPolygon(painter, entity, insert_data);
    }

    endInsertion(painter, insert_data);
}

void DxfViewer::setDrawingProperties(QPainter &painter, Entity *entity, bool forceSelected)
{
    int red=entity->redColor();
    int green=entity->greenColor();
    int blue=entity->blueColor();

    // Alter color if white.
    if (red==255 && green==255 && blue==255)
    {
        red=0;
        green=0;
        blue=0;
    }

    // Selected.
    if (entity->selected() || forceSelected)
    {
        red=255;
        green=0;
        blue=0;
    }

    QColor color;
    color.setRgb(red, green, blue);
    painter.setPen(color);
    painter.setBrush(Qt::NoBrush);

    QPen p=painter.pen();
    double width=entity->width();

    double w=_viewZoom->toScreenLength(width);
    p.setWidth(w);

    Qt::PenStyle style=penStyleFromLineType(entity->lineType());
    p.setStyle(style);
    painter.setPen(p);
}

void DxfViewer::drawPoint(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    Point2d *point=(Point2d *) entity;

    double X=applyInsertionTransformsX(point->x(), insert_data);
    double Y=applyInsertionTransformsY(point->y(), insert_data);

    painter.drawPoint(X, Y);
}

void DxfViewer::drawLine(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    Line* line=(Line*) entity;

    double X1=applyInsertionTransformsX(line->begin().x(), insert_data);
    double Y1=applyInsertionTransformsY(line->begin().y(), insert_data);

    double X2=applyInsertionTransformsX(line->end().x(), insert_data);
    double Y2=applyInsertionTransformsY(line->end().y(), insert_data);

    painter.drawLine(X1, Y1, X2, Y2);
}

void DxfViewer::drawBlock(bool draw_selected, QPainter &painter, Entity *entity)
{
    InsertData *insert_data=(InsertData*) entity;
    
    Block *block=insert_data->block();
    if (!block)
        return;

    if (!block->visible())
        return;

    drawEntities(draw_selected, painter, block, insert_data, insert_data->selected());
}

void DxfViewer::drawArc(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    Arc* arc=(Arc*) entity;

    double X=applyInsertionTransformsX(arc->xCenter()-arc->radius(), insert_data);
    double Y=applyInsertionTransformsY(arc->yCenter()+arc->radius(), insert_data);

    double w_diameter=_viewZoom->toScreenLength(2.0*arc->radius());
    if (insert_data)
        w_diameter=_viewZoom->toScreenLength(2.0*arc->radius()*insert_data->scaleX());
    double h_diameter=_viewZoom->toScreenLength(2.0*arc->radius());
    if (insert_data)
        h_diameter=_viewZoom->toScreenLength(2.0*arc->radius()*insert_data->scaleX());

    double angle1=arc->beginAngle()*16.0;
    double angle2=arc->endAngle()*16.0;

    if (angle1>angle2)
        angle2+=5760.0;

    painter.drawArc(X, Y, w_diameter, h_diameter, angle1, angle2-angle1);
}

void DxfViewer::drawCircle(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    Circle* circle=(Circle*) entity;

    double X=applyInsertionTransformsX(circle->xCenter()-circle->radius(), insert_data);
    double Y=applyInsertionTransformsY(circle->yCenter()+circle->radius(), insert_data);

    double X2=applyInsertionTransformsX(circle->xCenter()+circle->radius(), insert_data);
    double Y2=applyInsertionTransformsY(circle->yCenter()-circle->radius(), insert_data);

    double w_diameter=X2-X;
    double h_diameter=Y2-Y;

    if (circle->filled())
        painter.setBrush(painter.pen().color());

    painter.drawEllipse(X, Y, w_diameter, h_diameter);
}

void DxfViewer::drawPolylinePoint(QPainter &painter, const Vertex &v1, const Vertex &v2, InsertData *insert_data)
{
    double X1=applyInsertionTransformsX(v1.x(), insert_data);
    double Y1=applyInsertionTransformsY(v1.y(), insert_data);

    double X2=applyInsertionTransformsX(v2.x(), insert_data);
    double Y2=applyInsertionTransformsY(v2.y(), insert_data);

    if (v1.bulge()==0.0)
        painter.drawLine(X1, Y1, X2, Y2);

    else
    {
        painter.translate(X1, Y1);
        X2=X2-X1;
        Y2=Y2-Y1;

        double theta=atan2(Y2, X2)*180.0/M_PI;
        painter.rotate(theta);
        X2=sqrt(X2*X2+Y2*Y2);
        Y2=0;

        double alpha=std::fabs(4.0*atan(v1.bulge())*180.0/M_PI);
        double R=std::fabs(X2/2.0/sin(alpha/360.0*M_PI));

        double X0=X2/2.0-R;
        double Y0=sqrt(R*R-X2*X2/4.0)-R;

        double alpha_0=(90.0-alpha/2.0)*16.0;
        double alpha_1=(90.0+alpha/2.0)*16.0;

        if (v1.bulge()>0.0)
        {
            painter.translate(X2/2.0, 0);
            painter.rotate(180.0);
            painter.translate(-X2/2.0, 0);
        }

        painter.drawArc(X0, Y0, 2.0*R, 2.0*R, std::min(alpha_0, alpha_1), std::fabs(alpha_1-alpha_0));

        if (v1.bulge()>0.0)
        {
            painter.translate(X2/2.0, 0);
            painter.rotate(-180.0);
            painter.translate(-X2/2.0, 0);
        }

        painter.rotate(-theta);
        painter.translate(-X1, -Y1);
    }
}

void DxfViewer::drawText(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    Text *text=(Text*) entity;
    
    double X=applyInsertionTransformsX(text->x(), insert_data);
    double Y=applyInsertionTransformsY(text->y(), insert_data);

    double dX=0.0;
    double dY=0.0;

    int text_height=_viewZoom->toScreenLength(text->textHeight()*1.5);   //< HACK: 1.5
    if (text_height==0)
        return;

    QString string=QString::fromLocal8Bit(text->text().c_str());

    QFont f=painter.font();
    if (!text->font().empty())
        f.setFamily(text->font().c_str());
    f.setPixelSize(text_height);
    f.setBold(text->bold());
    f.setItalic(text->italic());
    painter.setFont(f);

    QFontMetrics metric(f);
    int w=metric.horizontalAdvance(string);
    int h=metric.height();

    Text::AttachmentPoint attachment=text->attachmentPoint();
    if (attachment==Text::TopCenter || attachment==Text::MiddleCenter || attachment==Text::BottomCenter)
        dX=-w/2.0;
    else if (attachment==Text::TopRight || attachment==Text::MiddleRight || attachment==Text::BottomRight)
        dX=-w;

    if (attachment==Text::MiddleLeft || attachment==Text::MiddleCenter || attachment==Text::MiddleRight)
        dY=h/2.0;
    else if (attachment==Text::BottomLeft || attachment==Text::BottomCenter || attachment==Text::BottomRight)
        dY=h;

    painter.translate(X, Y);
    painter.rotate(-text->angle());
    painter.translate(dX, dY);

    painter.drawText(0, 0, string);

    painter.translate(-dX, -dY);
    painter.rotate(text->angle());
    painter.translate(-X, -Y);
}

void DxfViewer::drawPolygon(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    int red=entity->redColor();
    int green=entity->greenColor();
    int blue=entity->blueColor();

    QColor color;
    color.setRgb(red, green, blue);
    painter.setBrush(color);

    Polygon *polygon=(Polygon*) entity;
    QPolygonF p;

    for (unsigned int i=0 ; i<polygon->size() ; i++)
    {
        Vertex &v1=polygon->at(i);

        double X1=applyInsertionTransformsX(v1.x(), insert_data);
        double Y1=applyInsertionTransformsY(v1.y(), insert_data);

        p << QPointF(X1, Y1);
    }
    
    painter.drawPolygon(p);
}

void DxfViewer::drawPolyline(QPainter &painter, Entity *entity, InsertData *insert_data)
{
    Polyline *line=(Polyline*) entity;

    if (line->size()<2)
        return;

    for (unsigned int i=0 ; i<line->size()-1 ; i++)
    {
        Vertex &v1=line->at(i);
        Vertex &v2=line->at(i+1);

        drawPolylinePoint(painter, v1, v2, insert_data);
    }

    if (line->isClosed())
        drawPolylinePoint(painter, line->at(line->size()-1), line->at(0), insert_data);
}

double DxfViewer::applyInsertionTransformsX(double x, InsertData *insert_data) const
{
    if (!insert_data)
        return _viewZoom->toScreenPosition(x,0).x();

    return _viewZoom->toScreenLength(x*insert_data->scaleX());
}

double DxfViewer::applyInsertionTransformsY(double y, InsertData *insert_data) const
{
    if (!insert_data)
        return _viewZoom->toScreenPosition(0, y).y();
    
    return _viewZoom->toScreenLength(-y*insert_data->scaleY());
}

void DxfViewer::prepareInsertion(QPainter &painter, InsertData *insert_data)
{
    if (!insert_data)
        return;

    Block *block = insert_data->block();
    if (!block)
        return;

    double dx = -block->x() + insert_data->insertionPointX();
    double dy = -block->y() + insert_data->insertionPointY();

    int dX=_viewZoom->toScreenPosition(dx, 0).x();
    int dY=_viewZoom->toScreenPosition(0, dy).y();

    painter.translate(dX, dY);
    painter.rotate(-insert_data->angle());
}

void DxfViewer::endInsertion(QPainter &painter, InsertData *insert_data)
{
    if (!insert_data)
        return;

    Block *block = insert_data->block();
    if (!block)
        return;

    double dx = -block->x() + insert_data->insertionPointX();
    double dy = -block->y() + insert_data->insertionPointY();

    int dX=_viewZoom->toScreenPosition(dx, 0).x();
    int dY=_viewZoom->toScreenPosition(0, dy).y();

    painter.rotate(insert_data->angle());
    painter.translate(-dX, -dY);
}

Qt::PenStyle DxfViewer::penStyleFromLineType(Entity::LineType type) const
{
    // Dash.
    if (type==Entity::Dashed)
        return Qt::DashLine;
    if (type==Entity::DashedX2)
        return Qt::DashLine;
    if (type==Entity::Dashed2)
        return Qt::DashLine;
    if (type==Entity::Phantom)
        return Qt::DashLine;
    if (type==Entity::PhantomX2)
        return Qt::DashLine;
    if (type==Entity::Phantom2)
        return Qt::DashLine;
    if (type==Entity::Center)
        return Qt::DashLine;
    if (type==Entity::CenterX2)
        return Qt::DashLine;
    if (type==Entity::Center2)
        return Qt::DashLine;

    // Dash & dot.
    if (type==Entity::DashDot)
        return Qt::DashDotLine;
    if (type==Entity::DashDotX2)
        return Qt::DashDotLine;
    if (type==Entity::DashDot2)
        return Qt::DashDotLine;

    // Dot.
    if (type==Entity::Dot)
        return Qt::DotLine;
    if (type==Entity::DotX2)
        return Qt::DotLine;
    if (type==Entity::Dot2)
        return Qt::DotLine;

    // Dash & dot-dot.
    if (type==Entity::Divide)
        return Qt::DashDotDotLine;
    if (type==Entity::DivideX2)
        return Qt::DashDotDotLine;
    if (type==Entity::Divide2)
        return Qt::DashDotDotLine;

    return Qt::SolidLine;
}
