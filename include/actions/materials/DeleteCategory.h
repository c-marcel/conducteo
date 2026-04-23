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

#ifndef _DELETE_CATEGORY_H
#define _DELETE_CATEGORY_H

#include <actions/UserAction.h>
#include <vector>
#include <string>

class DeleteMaterial;

class DeleteCategory: public UserAction
{

public:

    DeleteCategory(const std::string &category, const std::string &parentId, const std::string &name, const std::vector<DeleteCategory*> &child_categories, const std::vector<DeleteMaterial*> &child_materials);
    virtual ~DeleteCategory();

    void exec();
    void undo();

public:

    std::string     _id;
    std::string     _parentId;
    std::string     _name;

    std::vector<DeleteCategory*>    _categories;
    std::vector<DeleteMaterial*>    _materials;

};

#endif
