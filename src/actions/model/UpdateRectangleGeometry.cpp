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

#include <actions/model/UpdateRectangleGeometry.h>
#include <geometry/Line.h>
#include <model/Volume.h>
#include <model/Model.h>

UpdateRectangleGeometry::UpdateRectangleGeometry(Model *model, Volume *volume, double x, double y, double width, double height):
    _x(x),
    _y(y),
    _width(width),
    _height(height),
    _model(model)
{
    _volumeId=volume->id();
    _oldX=volume->xMin();
    _oldY=volume->yMin();
    _oldWidth=volume->xMax()-volume->xMin();
    _oldHeight=volume->yMax()-volume->yMin();
}

UpdateRectangleGeometry::~UpdateRectangleGeometry()
{
}

void UpdateRectangleGeometry::exec()
{
    storePhysics(_model);

    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    apply(_oldX, _oldY, _oldWidth, _oldHeight, _x, _y, _width, _height);

    volume->computeBounds();
    volume->computeLines();
    _model->recreateSurfaces();
}

void UpdateRectangleGeometry::undo()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    apply(_x, _y, _width, _height, _oldX, _oldY, _oldWidth, _oldHeight);

    volume->computeBounds();
    volume->computeLines();
    _model->recreateSurfaces();

    restorePhysics(_model);
}

void UpdateRectangleGeometry::apply(double xi, double yi, double widthi, double heighti, double xf, double yf, double widthf, double heightf)
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    double dx=xf-xi;
    double dy=yf-yi;
    double dwidth=widthf-widthi;
    double dheight=heightf-heighti;

    if (dx!=0.0)
    {
        for (unsigned int i=0 ; i<volume->size() ; i++)
        {
            Line *line=(Line*)volume->at(i);
            Point b=line->begin();
            Point e=line->end();
            
            b.setX(b.x()+dx);
            e.setX(e.x()+dx);

            line->setBegin(b, 0);
            line->setEnd(e, 0);
        }
    }
    else if (dy!=0.0)
    {
        for (unsigned int i=0 ; i<volume->size() ; i++)
        {
            Line *line=(Line*)volume->at(i);
            Point b=line->begin();
            Point e=line->end();
            
            b.setY(b.y()+dy);
            e.setY(e.y()+dy);

            line->setBegin(b, 0);
            line->setEnd(e, 0);
        }
    }
    else if (dwidth!=0.0)
    {
        for (unsigned int i=0 ; i<volume->size() ; i++)
        {
            Line *line=(Line*)volume->at(i);
            Point b=line->begin();
            Point e=line->end();
            
            if (b.x()==xi+widthi)
            {
                b.setX(xf+widthf);
                line->setBegin(b, 0);
            }
            
            if (e.x()==xi+widthi)
            {
                e.setX(xf+widthf);
                line->setEnd(e, 0);
            }
        }
    }
    else if (dheight!=0.0)
    {
        for (unsigned int i=0 ; i<volume->size() ; i++)
        {
            Line *line=(Line*)volume->at(i);
            Point b=line->begin();
            Point e=line->end();
            
            if (b.y()==yi+heighti)
            {
                b.setY(yf+heightf);
                line->setBegin(b, 0);
            }
            
            if (e.y()==yi+heighti)
            {
                e.setY(yf+heightf);
                line->setEnd(e, 0);
            }
        }
    }
}
