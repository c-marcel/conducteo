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

#include <actions/model/TranslateVolumeCorner.h>
#include <model/Volume.h>
#include <model/Model.h>

TranslateVolumeCorner::TranslateVolumeCorner(Model *model, Volume *volume, const Point &point, double dx, double dy):
    _dx(dx),
    _dy(dy),
    _model(model),
    _point(point)
{
    _volumeId=volume->id();
}

TranslateVolumeCorner::~TranslateVolumeCorner()
{
}

void TranslateVolumeCorner::exec()
{
    storePhysics(_model);
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;
    volume->translatePoint(_point, _dx, _dy);
    volume->computeBounds();
    volume->computeLines();
    _model->recreateSurfaces();
}

void TranslateVolumeCorner::undo()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        return;

    Point p;
    p.setX(_point.x()+_dx);
    p.setY(_point.y()+_dy);

    volume->translatePoint(p, -_dx, -_dy);
    volume->computeBounds();
    volume->computeLines();
    _model->recreateSurfaces();
    restorePhysics(_model);
}
