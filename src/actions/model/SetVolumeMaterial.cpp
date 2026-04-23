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

#include <actions/model/SetVolumeMaterial.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <model/Volume.h>
#include <model/Model.h>

SetVolumeMaterial::SetVolumeMaterial(Model *model, const std::string &materialId, double x, double y):
    _model(model),
    _materialId(materialId),
    _x(x),
    _y(y)
{
    Volume *volume=model->getVolumeAtPosition(x, y);
    if (volume && volume->material())
        _oldMaterialId=volume->material()->id();
}

SetVolumeMaterial::SetVolumeMaterial(Model *model, const std::string &materialId, Volume *volume):
    _model(model),
    _materialId(materialId),
    _volumeId(volume->id())
{
    Material *material=volume->material();
    if (material)
        _oldMaterialId=material->id();
}

SetVolumeMaterial::~SetVolumeMaterial()
{
}

void SetVolumeMaterial::exec()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        volume=_model->getVolumeAtPosition(_x, _y);
    if (volume)
        volume->setMaterial(Materials::instance()->materialFromId(_materialId));
    _model->recreateSurfaces();
}

void SetVolumeMaterial::undo()
{
    Volume *volume=_model->getVolumeById(_volumeId);
    if (!volume)
        volume=_model->getVolumeAtPosition(_x, _y);
    if (volume)
        volume->setMaterial(Materials::instance()->materialFromId(_oldMaterialId));
    _model->recreateSurfaces();
}
