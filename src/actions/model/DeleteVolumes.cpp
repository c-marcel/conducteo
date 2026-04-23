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

#include <actions/model/DeleteVolumes.h>
#include <actions/ActionsManager.h>
#include <model/Model.h>

DeleteVolumes::DeleteVolumes(Model *model, const std::vector<Volume *> &volumes):
    _model(model)
{
    for (unsigned int i=0 ; i<volumes.size() ; i++)
    {
        VolumeData data;
        data._volumeId=volumes.at(i)->id();
        data._oldVolume=*volumes.at(i);
        _data.push_back(data);
    }
}

DeleteVolumes::~DeleteVolumes()
{
}

void DeleteVolumes::exec()
{
    storePhysics(_model);
    for (unsigned int i=0 ; i<_data.size() ; i++)
    {
        _data.at(i)._stackPosition=_model->volumeStackPosition(_data.at(i)._volumeId);
        _model->removeVolume(_data.at(i)._volumeId);
    }
    _model->recreateSurfaces();
}

void DeleteVolumes::undo()
{
    if (_data.empty())
        return;

    for (int i=_data.size()-1 ; i>=0 ; i--)
    {
        Volume *volume=new Volume(_data.at(i)._oldVolume);
        volume->setId(_data.at(i)._volumeId);
        _model->insertVolume(volume, _data.at(i)._stackPosition);
    }
    restorePhysics(_model);
}
