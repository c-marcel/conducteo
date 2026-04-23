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

#include <actions/model/Copy2dModelTo1dModel.h>

Copy2dModelTo1dModel::Copy2dModelTo1dModel(Model *model):
    _model(model),
    _1dModel(0)
{
    // Copy old 1d model.
    Model *model1d=_model->model1d();
    if (model1d)
        _1dModel=new Model(*model1d);
}

Copy2dModelTo1dModel::~Copy2dModelTo1dModel()
{
    if (_1dModel)
        delete _1dModel;
}

void Copy2dModelTo1dModel::exec()
{
    Model *model1d=_model->model1d();

    // Replace 1d model.
    if (model1d)
    {
        _model->setModel1d(0);
        delete model1d;
    }

    Model *new_model1d=new Model(*_model);
    new_model1d->setCallback(0, 0);
    _model->setModel1d(new_model1d);
}

void Copy2dModelTo1dModel::undo()
{
    if (!_1dModel)
        return;

    // Replace 1d model.
    Model *model1d=_model->model1d();
    if (model1d)
    {
        _model->setModel1d(0);
        delete model1d;
    }

    Model *new_model1d=new Model(*_1dModel);
    new_model1d->setCallback(0, 0);
    _model->setModel1d(new_model1d);
}
