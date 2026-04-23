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

#include <actions/boundary-conditions/StorePhysicAction.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>
#include <databases/Environments.h>
#include <physics/Environment.h>
#include <model/Surface.h>
#include <model/Model.h>

StorePhysicAction::StorePhysicAction()
{
}

StorePhysicAction::~StorePhysicAction()
{
}

void StorePhysicAction::storePhysics(Model *model)
{
    _data.clear();

    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=model->std::vector<Surface*>::at(i);

        for (unsigned int j=0 ; j<surface->size() ; j++)
        {
            Line *line=surface->at(j);
            Point m=line->middlePoint();

            SurfaceStorageData data;
            data._x=m.x();
            data._y=m.y();

            if (surface->boundaryCondition())
                data._boundaryConditionId=surface->boundaryCondition()->id();
            if (surface->environment())
                data._environmentId=surface->environment()->name();

            _data.push_back(data);
        }
    }
}

void StorePhysicAction::restorePhysics(Model *model)
{
    // Set adiabatic & no environment for all surfaces.
    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=model->std::vector<Surface*>::at(i);
        surface->setBoundaryCondition(0);
        surface->setEnvironment(0);
    }

    // Set stored data.
    for (unsigned int i=0 ; i<_data.size() ; i++)
    {
        SurfaceStorageData &data=_data.at(i);
        Surface *surface=model->getSurfaceAtPosition(data._x, data._y, 1.0e-6);
        if (!surface)
            continue;

        BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(data._boundaryConditionId);
        surface->setBoundaryCondition(bc);

        Environment *env=Environments::instance()->environmentFromName(data._environmentId);
        surface->setEnvironment(env);
    }
}
