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

#include <databases/Materials.h>
#include <databases/MaterialsObserver.h>
#include <physics/Material.h>
#include <tools/ToolBox.h>

#define UnventilatedAirCavityIdentifier         "106D6720-211C-4090-8C60-7CED671C8580"
#define PartiallyVentilatedAirCavityIdentifier  "0BB7BBC8-79A2-4E71-A92F-F8E9033A630B"

Materials::Materials():
    _informObservers(true)
{
}

void Materials::loadDefaultMaterials()
{
    // Create air materials.
    Material *material1=new Material;
    material1->setName(_tr("UnventilatedAirCavity").toStdString());
    material1->setColor(158, 217, 255);
    material1->setId(UnventilatedAirCavityIdentifier);
    addMaterialToUserDb(material1);

    Material *material2=new Material;
    material2->setName(_tr("PartVentilatedAirCavity").toStdString());
    material2->setColor(17, 112, 255);
    material2->setId(PartiallyVentilatedAirCavityIdentifier);
    addMaterialToUserDb(material2);
}

void Materials::translate()
{
    Material* material1 = materialFromId(UnventilatedAirCavityIdentifier);
    if (material1)
        material1->setName(_tr("UnventilatedAirCavity").toStdString());
    materialModified(UnventilatedAirCavityIdentifier);

    Material* material2 = materialFromId(PartiallyVentilatedAirCavityIdentifier);
    if (material2)
        material2->setName(_tr("PartVentilatedAirCavity").toStdString());
    materialModified(PartiallyVentilatedAirCavityIdentifier);
}

Materials::~Materials()
{
    for (unsigned int i=0 ; i<_rt.size() ; i++)
        delete _rt.at(i);
    for (unsigned int i=0 ; i<_user.size() ; i++)
        delete _user.at(i);
}

void Materials::addMaterialToUserDb(Material *material)
{
    _user.push_back(material);
    materialAdded(material->id());
}

void Materials::deleteMaterialById(const std::string &id)
{
    for (unsigned int i=0 ; i<_user.size() ; i++)
    {
        if (_user.at(i)->id()==id)
        {
            delete _user.at(i);
            _user.erase(_user.begin()+i);
            break;
        }
    }
    for (unsigned int i=0 ; i<_rt.size() ; i++)
    {
        if (_rt.at(i)->id()==id)
        {
            delete _rt.at(i);
            _rt.erase(_rt.begin()+i);
            break;
        }
    }
    materialDeleted(id);
}

void Materials::addMaterialToRtDb(Material *material)
{
    _rt.push_back(material);
    materialAdded(material->id());
}

Material *Materials::materialFromId(const std::string &id) const
{
    for (unsigned int i=0 ; i<_user.size() ; i++)
        if (_user.at(i)->id()==id)
            return _user.at(i);
    for (unsigned int i=0 ; i<_rt.size() ; i++)
        if (_rt.at(i)->id()==id)
            return _rt.at(i);
    return 0;
}

Material *Materials::duplicate(Material *material) const
{
    for (unsigned int i=0 ; i<_user.size() ; i++)
        if (_user.at(i)->isEquivalentTo(material))
            return _user.at(i);
    for (unsigned int i=0 ; i<_rt.size() ; i++)
        if (_rt.at(i)->isEquivalentTo(material))
            return _rt.at(i);
    return 0;
}

void Materials::registerObserver(MaterialsObserver *observer)
{
    _observers.push_back(observer);
}

void Materials::unregisterObserver(MaterialsObserver *observer)
{
    for (unsigned int i=0 ; i<_observers.size() ; i++)
    {
        if (_observers.at(i)==observer)
        {
            _observers.erase(_observers.begin()+i);
            return;
        }
    }
}

void Materials::materialDeleted(const std::string &id)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->materialDeleted(id);
}

void Materials::materialAdded(const std::string &id)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->materialAdded(id);
}

void Materials::materialModified(const std::string &id)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->materialModified(id);
}

void Materials::informObservers(bool inform)
{
    _informObservers=inform;
}

void Materials::createCategory(const std::string &id, const std::string &parentId, const std::string &name)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->categoryAdded(id, parentId, name);
}

void Materials::deleteCategory(const std::string &id)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->categoryDeleted(id);
}

void Materials::renameCategory(const std::string &id, const std::string &old_name, const std::string &new_name)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->categoryRenamed(id, old_name, new_name);
}

std::vector<std::string> Materials::materialIdList() const
{
    std::vector<std::string> id;

    for (unsigned int i=0 ; i<_user.size() ; i++)
        id.push_back(_user.at(i)->id());
    for (unsigned int i=0 ; i<_rt.size() ; i++)
        id.push_back(_rt.at(i)->id());

    return id;
}

std::vector<Material*> Materials::materialListFromCategory(const std::string &category) const
{
	std::vector<Material*> out;

	for (unsigned int i = 0; i < _user.size(); i++)
	{
		Material *material = _user[i];

		if (material && material->category() == category)
			out.push_back(material);
	}

	return out;
}
