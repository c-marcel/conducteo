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

#include <actions/boundary-conditions/SetBoundaryConditionName.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>

SetBoundaryConditionName::SetBoundaryConditionName(BoundaryCondition *bc, const std::string &name)
{
    _alterResults = false;

    _bcId=bc->id();
    _newName=name;
    _oldName=bc->name();
}

SetBoundaryConditionName::~SetBoundaryConditionName()
{
}

void SetBoundaryConditionName::exec()
{
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(_bcId);
    if (!bc)
        return;
    
    bc->setName(_newName);
}

void SetBoundaryConditionName::undo()
{
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(_bcId);
    if (!bc)
        return;
    
    bc->setName(_oldName);
}
