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

#ifndef MATERIALS_H
#define MATERIALS_H

#include <tools/Singleton.h>
#include <vector>
#include <string>

class MaterialsObserver;
class Material;

class Materials: public Singleton<Materials>
{

public:

    Materials();
    virtual ~Materials();

    void informObservers(bool inform);
    void registerObserver(MaterialsObserver *observer);
    void unregisterObserver(MaterialsObserver *observer);

    void loadDefaultMaterials();
    void addMaterialToUserDb(Material *material);
    void addMaterialToRtDb(Material *material);

    void deleteMaterialById(const std::string &id);
    void materialModified(const std::string &id);

    Material *materialFromId(const std::string &id) const;
    Material *duplicate(Material *material) const;

    void createCategory(const std::string &id, const std::string &parentId, const std::string &name);
    void deleteCategory(const std::string &id);
    void renameCategory(const std::string &id, const std::string &old_name, const std::string &new_name);

    std::vector<std::string> materialIdList() const;
	std::vector<Material*> materialListFromCategory(const std::string &category) const;

private:

    void materialDeleted(const std::string &id);
    void materialAdded(const std::string &id);

    std::vector<Material*>          _rt;
    std::vector<Material*>          _user;
    std::vector<MaterialsObserver*> _observers;

    bool                            _informObservers;

};

#endif
