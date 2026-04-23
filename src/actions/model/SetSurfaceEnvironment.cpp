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

#include <actions/model/SetSurfaceEnvironment.h>
#include <databases/Environments.h>
#include <physics/Environment.h>
#include <model/Surface.h>
#include <model/Model.h>

SetSurfaceEnvironment::SetSurfaceEnvironment(Model *model, const std::string &environmentName, double x, double y, double precision):
    _model(model),
    _environmentName(environmentName),
    _x(x),
    _y(y),
    _precision(precision)
{
    Surface *surface=model->getSurfaceAtPosition(x, y, precision);
    if (surface && surface->environment())
        _oldEnvironmentName=surface->environment()->name();
}

SetSurfaceEnvironment::~SetSurfaceEnvironment()
{
}

void SetSurfaceEnvironment::exec()
{
    Surface *surface=_model->getSurfaceAtPosition(_x, _y, _precision);
    if (surface)
        surface->setEnvironment(Environments::instance()->environmentFromName(_environmentName));
}

void SetSurfaceEnvironment::undo()
{
    Surface *surface=_model->getSurfaceAtPosition(_x, _y, _precision);
    if (surface)
        surface->setEnvironment(Environments::instance()->environmentFromName(_oldEnvironmentName));
}
