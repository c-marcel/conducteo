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

#include <actions/model/TranslateVolumeBorder.h>
#include <geometry/Path.h>
#include <model/Volume.h>
#include <model/Model.h>

TranslateVolumeBorder::TranslateVolumeBorder(Model *model, Volume *volume, Path *path, double dx, double dy):
    _dx(dx),
    _dy(dy),
    _model(model)
{
    _volumeId=volume->id();
    _begin=path->begin();
    _end=path->end();
}

TranslateVolumeBorder::~TranslateVolumeBorder()
{
}

void TranslateVolumeBorder::exec()
{
    storePhysics(_model);
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;
    volume->translateBorder(_begin, _end, _dx, _dy);
    volume->computeBounds();
    volume->computeLines();
    _model->recreateSurfaces();
}

void TranslateVolumeBorder::undo()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    Point p1;
    p1.setX(_begin.x()+_dx);
    p1.setY(_begin.y()+_dy);

    Point p2;
    p2.setX(_end.x()+_dx);
    p2.setY(_end.y()+_dy);

    volume->translateBorder(p1, p2, -_dx, -_dy);
    volume->computeBounds();
    volume->computeLines();
    _model->recreateSurfaces();
    restorePhysics(_model);
}
