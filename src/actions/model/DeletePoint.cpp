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

#include <actions/model/DeletePoint.h>
#include <geometry/Path.h>
#include <geometry/Line.h>
#include <geometry/Arc.h>
#include <model/Volume.h>
#include <model/Model.h>

DeletePoint::DeletePoint(Model *model, Volume *volume, const Point &point):
    _model(model),
    _point(point),
    _canceled(false)
{
    _volumeId=volume->id();

    for (unsigned int i=0 ; i<volume->size() ; i++)
    {
        Path *path=volume->at(i);
        if (path->type()==Path::LinePath)
        {
            Line *line=(Line*) path;
            Line *copy=new Line(*line);
            _path.push_back(copy);
        }
        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            Arc *copy=new Arc(*arc);
            _path.push_back(copy);
        }
    }
}

DeletePoint::~DeletePoint()
{
    for (unsigned int i=0 ; i<_path.size() ; i++)
        delete _path.at(i);
    _path.resize(0);
}

void DeletePoint::exec()
{
    storePhysics(_model);

    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    if (volume->size()<=3)
    {
        _canceled=true;
        return;
    }

    Volume v=*volume;
    v.removePoint(_point);

    if (!v.isClosed() || v.isSelfIntersected())
    {
        _canceled=true;
        return;
    }

    volume->removePoint(_point);
    volume->computeBounds();
    volume->computeLines();

    _model->recreateSurfaces();
}

void DeletePoint::undo()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    for (unsigned int i=0 ; i<volume->size() ; i++)
    {
        Path *path=volume->at(i);
        delete path;
    }
    volume->resize(0);
    
    for (unsigned int i=0 ; i<_path.size() ; i++)
    {
        Path *path=_path.at(i);
        if (path->type()==Path::LinePath)
        {
            Line *line=(Line*) path;
            Line *copy=new Line(*line);
            volume->push_back(copy);
        }
        else if (path->type()==Path::ArcPath)
        {
            Arc *arc=(Arc*) path;
            Arc *copy=new Arc(*arc);
            volume->push_back(copy);
        }
    }

    _model->recreateSurfaces();
    restorePhysics(_model);
}

bool DeletePoint::canceledAction() const
{
    return _canceled;
}
