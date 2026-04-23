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

#include <actions/model/RotateVolumes.h>
#include <model/Volume.h>
#include <model/Model.h>

RotateVolumes::RotateVolumes(Model *model, double x, double y, double angle):
    _angle(angle),
    _model(model),
    _x(x),
    _y(y)
{
    for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=model->std::vector<Volume*>::at(i);
        if (volume->selected())
            _volumes.push_back(volume->id());
    }
}

RotateVolumes::~RotateVolumes()
{
}

void RotateVolumes::exec()
{
    storePhysics(_model);
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        Volume *volume=_model->getVolumeById(_volumes.at(i));
        if (volume)
            volume->rotate(_x, _y, _angle);
    }
    _model->recreateSurfaces();
}

void RotateVolumes::undo()
{
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        Volume *volume=_model->getVolumeById(_volumes.at(i));
        if (volume)
            volume->rotate(_x, _y, -_angle);
    }
    _model->recreateSurfaces();
    restorePhysics(_model);
}
