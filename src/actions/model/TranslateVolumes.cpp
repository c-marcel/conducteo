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

#include <actions/model/TranslateVolumes.h>
#include <model/Volume.h>
#include <model/Model.h>

TranslateVolumes::TranslateVolumes(Model *model, double dx, double dy):
    _dx(dx),
    _dy(dy),
    _model(model)
{
    for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=model->std::vector<Volume*>::at(i);
        if (volume->selected())
            _volumes.push_back(volume->id());
    }
}

TranslateVolumes::~TranslateVolumes()
{
}

void TranslateVolumes::exec()
{
    storePhysics(_model);
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        Volume *volume=_model->getVolumeById(_volumes.at(i));
        if (volume)
            volume->translate(_dx, _dy);
    }
    _model->recreateSurfaces();
}

void TranslateVolumes::undo()
{
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        Volume *volume=_model->getVolumeById(_volumes.at(i));
        if (volume)
            volume->translate(-_dx, -_dy);
    }
    _model->recreateSurfaces();
    restorePhysics(_model);
}
