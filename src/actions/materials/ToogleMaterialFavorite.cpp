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

#include <actions/materials/ToogleMaterialFavorite.h>
#include <databases/Materials.h>
#include <physics/Material.h>

ToogleMaterialFavorite::ToogleMaterialFavorite(const std::string &id):
    _materialId(id)
{
    _alterResults = false;
}

ToogleMaterialFavorite::~ToogleMaterialFavorite()
{
}

void ToogleMaterialFavorite::exec()
{
    Material *material=Materials::instance()->materialFromId(_materialId);
    if (!material)
        return;

    _oldValue=material->favorite();
    material->setFavorite(!_oldValue);
    Materials::instance()->materialModified(_materialId);
}

void ToogleMaterialFavorite::undo()
{
    Material *material=Materials::instance()->materialFromId(_materialId);
    if (!material)
        return;

    material->setFavorite(_oldValue);
    Materials::instance()->materialModified(_materialId);
}
