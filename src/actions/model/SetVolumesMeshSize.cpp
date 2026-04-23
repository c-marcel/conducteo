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

#include <actions/model/SetVolumesMeshSize.h>
#include <model/Volume.h>
#include <model/Model.h>

SetVolumesMeshSize::SetVolumesMeshSize(Model *model, const std::vector<Volume*> &volumes, double mesh_size):
    _model(model),
    _meshSize(mesh_size)
{
    for (unsigned int i=0 ; i<volumes.size() ; i++)
    {
        _oldMeshSize.push_back(volumes.at(i)->cellsize());
        _volumes.push_back(volumes.at(i)->id());
    }
}

SetVolumesMeshSize::~SetVolumesMeshSize()
{
}

void SetVolumesMeshSize::exec()
{
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        Volume *volume=_model->getVolumeById(_volumes.at(i));
        if (volume)
            volume->setCellSize(_meshSize, 0);
    }
}

void SetVolumesMeshSize::undo()
{
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        Volume *volume=_model->getVolumeById(_volumes.at(i));
        if (volume)
            volume->setCellSize(_oldMeshSize.at(i), 0);
    }
}
