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

#include <actions/model/AddEllipse.h>
#include <actions/ActionsManager.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <model/Ellipse.h>
#include <model/Model.h>

AddEllipse::AddEllipse(Model *model, Ellipse *ellipse):
        _model(model)
{
    _x=ellipse->x();
    _y=ellipse->y();
    _horizontalRadius=ellipse->horizontalRadius();
    _verticalRadius=ellipse->verticalRadius();
    if (ellipse->material())
        _materialId=ellipse->material()->id();
}

AddEllipse::~AddEllipse()
{
}

void AddEllipse::exec()
{
    storePhysics(_model);
    Ellipse *volume=new Ellipse;
    volume->create(_x, _y, _horizontalRadius, _verticalRadius);

    Material *material=Materials::instance()->materialFromId(_materialId);
    volume->setMaterial(material);

    _model->addVolume(volume);

    if (!_volumeId.empty())
        volume->setId(_volumeId);
    _volumeId=volume->id();
}

void AddEllipse::undo()
{
    _model->removeVolume(_volumeId);
    _model->recreateSurfaces();
    restorePhysics(_model);
}
