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

#ifndef _SET_MATERIAL_MU_H
#define _SET_MATERIAL_MU_H

#include <actions/UserAction.h>
#include <string>

class Material;

class SetMaterialMu: public UserAction
{
public:
    SetMaterialMu(Material *material, double mu);
    virtual ~SetMaterialMu();

    void exec();
    void undo();

private:
    std::string _materialId;
    double      _newMu;
    double      _oldMu;
};

#endif
