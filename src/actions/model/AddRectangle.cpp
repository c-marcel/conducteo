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

#include <actions/model/AddRectangle.h>
#include <actions/ActionsManager.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <model/Rectangle.h>
#include <model/Model.h>

AddRectangle::AddRectangle(Model *model, Rectangle *rectangle):
        _model(model)
{
    _x=rectangle->x();
    _y=rectangle->y();
    _width=rectangle->width();
    _height=rectangle->height();
    if (rectangle->material())
        _materialId=rectangle->material()->id();
}

AddRectangle::~AddRectangle()
{
}

void AddRectangle::exec()
{
    storePhysics(_model);
    Rectangle *volume=new Rectangle;
    volume->create(_x, _y, _width, _height);

    Material *material=Materials::instance()->materialFromId(_materialId);
    volume->setMaterial(material);

    _model->addVolume(volume);
    
    if (!_volumeId.empty())
        volume->setId(_volumeId);
    _volumeId=volume->id();
}

void AddRectangle::undo()
{
    _model->removeVolume(_volumeId);
    _model->recreateSurfaces();
    restorePhysics(_model);
}
