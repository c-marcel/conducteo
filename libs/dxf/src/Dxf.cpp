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

#include <Dxf.h>
#include <libdxf/dl_dxf.h>
#include <InsertData.h>
#include <TextStyle.h>
#include <Polyline.h>
#include <Polygon.h>
#include <Point2d.h>
#include <Circle.h>
#include <Entity.h>
#include <Layer.h>
#include <fstream>
#include <Block.h>
#include <Line.h>
#include <Text.h>
#include <Arc.h>

using namespace DxfReader;
using namespace std;

Dxf::Dxf():
    _currentBlock(0),
    _currentEntity(0),
    _visible(true),
    _unitConversion(0.001),
    _zOrder(0)
{
}

Dxf::~Dxf()
{
    for (unsigned int i=0; i<size(); i++)
        delete at(i);
    for (unsigned int i=0; i<_textStyles.size(); i++)
        delete _textStyles.at(i);
    for (unsigned int i=0; i<_blocks.size(); i++)
        delete _blocks.at(i);
}

void Dxf::loadFromFile(const std::string &filename)
{
    _dxfStream = "";

    std::ifstream infile(filename.c_str());

    while (infile)
        _dxfStream.push_back(infile.get());

    infile.close();

    _zOrder = 0;

    loadFromStream(_dxfStream);
}

void Dxf::loadFromStream(const std::string &data)
{
    DL_Dxf dxf;

    std::stringstream stream(data);

    if (!dxf.in(stream, this))
        return;

    // Finalize insert data & block associations.
    finalizeInsertDataAndBlocks();

    _dxfStream=data;
}

std::string Dxf::data() const
{
    return _dxfStream;
}

Dxf::Dxf(const Dxf &)
{
    // Do nothing.
}

Dxf& Dxf::operator=(const Dxf &)
{
    // Do nothing.
    return *this;
}

void Dxf::addEntityToCurrentLayer(Entity* entity)
{
    std::string layer=attributes.getLayer();
    for (unsigned int i=0; i<size(); i++)
    {
        Layer *dxf_layer=at(i);
        if (layer==dxf_layer->getName())
        {
            dxf_layer->push_back(entity);
            entity->setLayer(dxf_layer);
            break;
        }
    }
}

double Dxf::minimalX() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Layer *layer=at(i);
        if (layer->isVisible())
        {
            if (layer->minimalX()<v || !defined)
            {
                v=layer->minimalX();
                defined=true;
            }
        }
    }
    return v;
}

double Dxf::minimalY() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Layer *layer=at(i);
        if (layer->isVisible())
        {
            if (layer->minimalY()<v || !defined)
            {
                v=layer->minimalY();
                defined=true;
            }
        }
    }
    return v;
}

double Dxf::maximalX() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Layer *layer=at(i);
        if (layer->isVisible())
        {
            if (layer->maximalX()>v || !defined)
            {
                v=layer->maximalX();
                defined=true;
            }
        }
    }
    return v;
}

double Dxf::maximalY() const
{
    double v=0.0;
    bool defined=false;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Layer *layer=at(i);
        if (layer->isVisible())
        {
            if (layer->maximalY()>v || !defined)
            {
                v=layer->maximalY();
                defined=true;
            }
        }
    }
    return v;
}

bool Dxf::visible() const
{
    return _visible;
}

void Dxf::setVisible(bool visible)
{
    _visible=visible;
}

void Dxf::defineEntityLine(Entity *entity)
{
    int width=attributes.getWidth();

    if (width==-1)
        entity->setWidth(Entity::ByLayerWidth);
    else if (width==-2)
        entity->setWidth(Entity::ByBlockWidth);
    else if (width==-3)
        entity->setWidth(Entity::DefaultWidth);
    else if (width>0)
        entity->setWidth(Entity::CustomWidth, width*_unitConversion);

    Entity::LineType type=Entity::lineTypeFromString(attributes.getLineType());
    entity->setLineType(type);
}

void Dxf::defineEntityColor(Entity *entity)
{
    int color=attributes.getColor();
    int color24 = attributes.getColor24();

    if (color==0)
    {
        Block *block=entity->block();

        if (block)
        {
            int red=block->redColor();
            int green=block->greenColor();
            int blue=block->blueColor();
            entity->setColor(red, green, blue);
        }
    }

    else if (color==256)
    {
        Layer *layer=entity->layer();
        
        if (layer)
        {
            int red=layer->redColor();
            int green=layer->greenColor();
            int blue=layer->blueColor();
            entity->setColor(red, green, blue);
        }
    }

    else
    {
        // Use indexed color.
        if (color24 == -1)
        {
            int red = int(dxfColors[color][0] * 255.0);
            int green = int(dxfColors[color][1] * 255.0);
            int blue = int(dxfColors[color][2] * 255.0);

            entity->setColor(red, green, blue);
        }

        // True color.
        else
        {
            int blue = color24 & 0xff;

            color24 = color24 >> 8;
            int green = color24 & 0xff;

            color24 = color24 >> 8;
            int red = color24 & 0xff;

            entity->setColor(red, green, blue);
        }
    }
}

void Dxf::addLayer(const DL_LayerData& data) 
{
    Layer *layer=new Layer;
    layer->setName(data.name);

    int color=attributes.getColor();
    if (color>255)
        color=0;
    int red=int(dxfColors[color][0]*255.0);
    int green=int(dxfColors[color][1]*255.0);
    int blue=int(dxfColors[color][2]*255.0);
    layer->setColor(red, green, blue);

    int width=attributes.getWidth();
    if (width>0)
        layer->setDefaultWidth(width*0.00001);

    std::string lineType=attributes.getLineType();
    layer->setDefaultLineType(Entity::lineTypeFromString(lineType));

    if (data.flags & 0x01)
        layer->setVisible(false);

    push_back(layer);
}

void Dxf::addPoint(const DL_PointData &data) 
{
    Point2d *point=new Point2d;
    point->setX(data.x*_unitConversion);
    point->setY(data.y*_unitConversion);

    processEntity(point);
}

void Dxf::addLine(const DL_LineData &data) 
{
    Point2d begin;
    begin.setX(data.x1*_unitConversion);
    begin.setY(data.y1*_unitConversion);

    Point2d end;
    end.setX(data.x2*_unitConversion);
    end.setY(data.y2*_unitConversion);

    Line *line=new Line;
    line->setBegin(begin);
    line->setEnd(end);
    
    processEntity(line);
}

void Dxf::addBlock(const DL_BlockData &data)
{
    Block *block=new Block;
    block->setName(data.name);
    block->setX(data.bpx*_unitConversion);
    block->setY(data.bpy*_unitConversion);

    _blocks.push_back(block);
    _currentBlock=block;
}

void Dxf::endBlock()
{
    _currentBlock=0;
}

void Dxf::addArc(const DL_ArcData &data) 
{
    Arc *arc=new Arc;
    arc->setCenter(data.cx*_unitConversion, data.cy*_unitConversion);
    arc->setRadius(data.radius*_unitConversion);
    arc->setBeginAngle(data.angle1);
    arc->setEndAngle(data.angle2);

    processEntity(arc);
}

void Dxf::addCircle(const DL_CircleData &data) 
{
    Circle *circle=new Circle;
    circle->setCenter(data.cx*_unitConversion, data.cy*_unitConversion);
    circle->setRadius(data.radius*_unitConversion);

    processEntity(circle);
}

void Dxf::addPolyline(const DL_PolylineData &data)
{
    if (_currentEntity)
        delete _currentEntity;
    _currentEntity=new Polyline;

    Polyline *line=(Polyline*) _currentEntity;
    if (data.flags & 0x01)
        line->setClosed(true);
}

void Dxf::addVertex(const DL_VertexData &data)
{
    if (!_currentEntity)
        return;
    if (_currentEntity->getType()!=Entity::TypePolyline)
        return;

    Polyline *line=(Polyline*) _currentEntity;
    
    Vertex vertex;
    vertex.setPosition(data.x*_unitConversion, data.y*_unitConversion);
    vertex.setBulge(data.bulge);
    line->push_back(vertex);
}

void Dxf::endEntity()
{
    if (!_currentEntity)
        return;

    _currentEntity->setZOrder(_zOrder);

    processEntity(_currentEntity);

    _currentEntity = 0;

    _zOrder++;
}

void Dxf::addTextStyle(const DL_StyleData &data)
{
    TextStyle *style=new TextStyle;
    style->setName(data.name);
    style->setFontName(data.primaryFontFile);
    style->setBold(data.bold);
    style->setItalic(data.italic);
    style->setDirection(data.flags & 0x04 ? TextStyle::Vertical : TextStyle::Horizontal);
    _textStyles.push_back(style);
}

void Dxf::addText(const DL_TextData &data)
{
    Text *text=new Text;
    text->setText(data.text);
    text->setAngle(data.angle/M_PI*180.0);
    text->setAnchorPoint(data.ipx*_unitConversion, data.ipy*_unitConversion);
    text->setTextHeight(data.height*_unitConversion);
    associateTextStyle(text, data.style);
    processEntity(text);
}

void Dxf::addMText(const DL_MTextData &data)
{
    Text *text=new Text;
    text->setAnchorPoint(data.ipx*_unitConversion, data.ipy*_unitConversion);
    text->setTextHeight(data.height*_unitConversion);
    text->setTextWidth(data.width*_unitConversion);
    text->setAttachmentPointValue(data.attachmentPoint);
    text->setAngle(data.angle/M_PI*180.0);
    associateTextStyle(text, data.style);
    processEntity(text);

    _currentText+=data.text;
    text->parseMText(_currentText);
    _currentText.clear();
}

void Dxf::addMTextChunk(const std::string &data)
{
    _currentText+=data;
}

void Dxf::addDimLinear(const DL_DimensionData &dimensions, const DL_DimLinearData &data)
{
    Vertex P1;
    P1.setPosition(data.dpx1*_unitConversion, data.dpy1*_unitConversion);

    Vertex P2;
    P2.setPosition(data.dpx2*_unitConversion, data.dpy2*_unitConversion);

    Vertex C;
    C.setPosition(dimensions.dpx*_unitConversion, dimensions.dpy*_unitConversion);

    double theta=data.angle/180.0*M_PI;
    double cos_theta=cos(theta);
    double sin_theta=sin(theta);

    Vertex A;
    double Ax=C.x()*sin_theta*sin_theta+P1.y()*cos_theta*sin_theta-C.y()*cos_theta*sin_theta+P1.x()*cos_theta*cos_theta;

    double Ay=0.0;
    if (fabs(cos_theta)<0.5)
        Ay=(-Ax*cos_theta+P1.x()*cos_theta+P1.y()*sin_theta)/sin_theta;

    else
        Ay=(Ax*sin_theta-C.x()*sin_theta+C.y()*cos_theta)/cos_theta;

    A.setPosition(Ax, Ay);

    Vertex B;
    double Bx=C.x()*sin_theta*sin_theta+P2.y()*cos_theta*sin_theta-C.y()*cos_theta*sin_theta+P2.x()*cos_theta*cos_theta;

    double By=0.0;
    if (fabs(cos_theta)<0.5)
        By=(-Bx*cos_theta+P2.x()*cos_theta+P2.y()*sin_theta)/sin_theta;

    else
        By=(Bx*sin_theta-C.x()*sin_theta+C.y()*cos_theta)/cos_theta;

    B.setPosition(Bx, By);

    Block *block=new Block;

    Polyline *line=new Polyline;
    line->setBlock(block);
    line->push_back(P1);
    line->push_back(A);
    line->push_back(B);
    line->push_back(P2);
    block->push_back(line);

    Text *text=new Text;
    text->setText(dimensions.text);
    text->setAngle(data.angle);
    text->setAnchorPoint(dimensions.mpx*_unitConversion, dimensions.mpy*_unitConversion);
    text->setBlock(block);
    associateTextStyle(text, dimensions.style);
    block->push_back(text);
    text->setTextHeight(2);
    text->setAttachmentPointValue(dimensions.attachmentPoint);

    if (dimensions.text.empty())
    {
        double length=sqrt((Ax-Bx)*(Ax-Bx)+(Ay-By)*(Ay-By));
        std::ostringstream s;
        s << length;
        text->setText(s.str());
    }

    double d=2.0;
    double r=0.5;

    double n_x=A.x()-B.x();
    double n_y=A.y()-B.y();
    double n_length=sqrt(n_x*n_x+n_y*n_y);
    n_x=n_x/n_length;
    n_y=n_y/n_length;

    double M_x=B.x()+n_x*d;
    double M_y=B.y()+n_y*d;

    double n_ortho_x=-n_y;
    double n_ortho_y=n_x;

    double N1_x=M_x+n_ortho_x*r;
    double N1_y=M_y+n_ortho_y*r;
    double N2_x=M_x-n_ortho_x*r;
    double N2_y=M_y-n_ortho_y*r;

    Vertex N1;
    N1.setPosition(N1_x, N1_y);

    Vertex N2;
    N2.setPosition(N2_x, N2_y);

    Polygon *arrow1=new Polygon;
    arrow1->push_back(B);
    arrow1->push_back(N1);
    arrow1->push_back(N2);
    arrow1->setBlock(block);
    block->push_back(arrow1);

    M_x=A.x()-n_x*d;
    M_y=A.y()-n_y*d;

    N1_x=M_x+n_ortho_x*r;
    N1_y=M_y+n_ortho_y*r;
    N2_x=M_x-n_ortho_x*r;
    N2_y=M_y-n_ortho_y*r;

    N1.setPosition(N1_x, N1_y);
    N2.setPosition(N2_x, N2_y);

    Polygon *arrow2=new Polygon;
    arrow2->push_back(A);
    arrow2->push_back(N1);
    arrow2->push_back(N2);
    arrow2->setBlock(block);
    block->push_back(arrow2);

    InsertData *insert=new InsertData;
    insert->setBlock(block);
    _blocks.push_back(block);
    processEntity(insert, true);
}

void Dxf::addSolid(const DL_SolidData &data)
{
    Polygon *polygon=new Polygon;

    Vertex v1;
    v1.setPosition(data.x[0]*_unitConversion, data.y[0]*_unitConversion);
    polygon->push_back(v1);

    Vertex v2;
    v2.setPosition(data.x[1]*_unitConversion, data.y[1]*_unitConversion);
    polygon->push_back(v2);

    Vertex v3;
    v3.setPosition(data.x[2]*_unitConversion, data.y[2]*_unitConversion);
    polygon->push_back(v3);

    Vertex v4;
    v4.setPosition(data.x[3]*_unitConversion, data.y[3]*_unitConversion);
    if (data.x[2]!=data.x[3] && data.y[2]!=data.y[3])
        polygon->push_back(v4);

    processEntity(polygon);
}

void Dxf::processEntity(Entity *entity, bool forceToLayer)
{
    if (!_currentBlock || forceToLayer)
        addEntityToCurrentLayer(entity);
    else
        addEntityToCurrentBlock(entity);

    defineEntityColor(entity);
    defineEntityLine(entity);
}

void Dxf::addEntityToCurrentBlock(Entity *entity)
{
    if (!_currentBlock)
        return;

    _currentBlock->push_back(entity);
    entity->setBlock(_currentBlock);
}

void Dxf::addInsert(const DL_InsertData& data)
{
    InsertData *insert_data=new InsertData;

    insert_data->setAngle(data.angle);
    insert_data->setInsertionPoint(data.ipx*_unitConversion, data.ipy*_unitConversion);
    insert_data->setScaleFactors(data.sx, data.sy);
    insert_data->setBlockName(data.name);

    processEntity(insert_data, true);
}

Block *Dxf::getBlockByName(const std::string &name) const
{
    for (unsigned int i=0 ; i<_blocks.size() ; i++)
        if (_blocks.at(i)->name()==name)
            return _blocks.at(i);
    return 0;
}

void Dxf::associateTextStyle(Text *text, const std::string &style_name)
{
    for (unsigned int i=0 ; i<_textStyles.size() ; i++)
    {
        TextStyle *style=_textStyles.at(i);
        if (style->name()==style_name)
        {
            text->setStyle(style);
            break;
        }
    }
}

void Dxf::finalizeInsertDataAndBlocks()
{
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Layer *layer = at(i);
        if (!layer)
            continue;

        for (unsigned int j=0 ; j<layer->size() ; j++)
        {
            Entity *entity = layer->at(j);
            if (!entity)
                continue;

            if (entity->getType() != Entity::TypeInsertData)
                continue;

            InsertData *insert = dynamic_cast<InsertData*>(entity);
            if (!insert)
                continue;

            std::string blockName = insert->getBlockName();
            Block *block = getBlockByName(blockName);
            if (!block)
                continue;

            insert->setBlock(block);
        }
    }
}

void Dxf::setVariableInt(const std::string &name, int value, int code)
{
    if (name=="$INSUNITS")
    {
        // mm.
        if (value==4)
            _unitConversion=0.001;

        // cm.
        else if (value==5)
            _unitConversion=0.01;

        // m.
        else if (value==6)
            _unitConversion=1.0;

        // km.
        else if (value==7)
            _unitConversion=1000.0;

        // dm.
        else if (value==14)
            _unitConversion=0.1;

        // Dm.
        else if (value==15)
            _unitConversion=10.0;

        // hm.
        else if (value==16)
            _unitConversion=100.0;
    }
}
