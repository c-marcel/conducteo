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

#include <actions/model/SendToBackVolume.h>
#include <model/Volume.h>
#include <model/Model.h>

SendToBackVolume::SendToBackVolume(Model *model, Volume *volume):
    _model(model),
    _volumeId(volume->id()),
    _stackPosition(0)
{
    _stackPosition=model->volumeStackPosition(volume);
}

SendToBackVolume::~SendToBackVolume()
{
}

void SendToBackVolume::exec()
{
    storePhysics(_model);
    Volume *volume=_model->getVolumeById(_volumeId);
    if (volume)
    {
        _model->setVolumeStackPosition(volume, 0);
        _model->recreateSurfaces();
    }
}

void SendToBackVolume::undo()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (volume)
    {
        _model->setVolumeStackPosition(volume, _stackPosition);
        _model->recreateSurfaces();
    }
    restorePhysics(_model);
}
