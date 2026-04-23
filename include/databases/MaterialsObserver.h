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

#ifndef MATERIALS_OBSERVER_H
#define MATERIALS_OBSERVER_H

#include <string>

class MaterialsObserver
{

public:

    MaterialsObserver();
    virtual ~MaterialsObserver();

    virtual void materialDeleted(const std::string &id)=0;
    virtual void materialAdded(const std::string &id)=0;
    virtual void materialModified(const std::string &id)=0;

    virtual void categoryDeleted(const std::string &id)=0;
    virtual void categoryAdded(const std::string &id, const std::string &parentId, const std::string &name)=0;
    virtual void categoryRenamed(const std::string &id, const std::string &old_name, const std::string &new_name)=0;

};

#endif
