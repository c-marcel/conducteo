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

#ifndef _STORE_PHYSIC_ACTION_H
#define _STORE_PHYSIC_ACTION_H

#include <actions/UserAction.h>
#include <vector>
#include <string>

class SurfaceStorageData
{

public:

    double      _x;
    double      _y;
    std::string _boundaryConditionId;
    std::string _environmentId;

};

class Model;

class StorePhysicAction: public UserAction
{

public:

    StorePhysicAction();
    virtual ~StorePhysicAction();

protected:

    void storePhysics(Model *model);
    void restorePhysics(Model *model);

private:

    std::vector<SurfaceStorageData> _data;

};

#endif
