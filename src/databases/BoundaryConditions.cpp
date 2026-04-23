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

#include <databases/BoundaryConditions.h>
#include <databases/BoundaryConditionsObserver.h>
#include <physics/BoundaryCondition.h>

BoundaryConditions::BoundaryConditions():
    _informObservers(true)
{
    // Create default boundary conditions.
    BoundaryCondition *bc0=new BoundaryCondition;
    bc0->setName("Adiabatique");
    bc0->setResistance(InfiniteResistance, 0);
    bc0->setTemperature(0.0);
    bc0->setColor(0, 0, 0);
    bc0->setId("");
    _standard.push_back(bc0);

    BoundaryCondition *bc1=new BoundaryCondition;
    bc1->setName("Extérieur");
    bc1->setResistance(0.04, 0);
    bc1->setTemperature(0.0);
    bc1->setColor(0, 0, 255);
    bc1->setId("BEB3A51D-800D-42A4-94E1-CA053B844276");
    _standard.push_back(bc1);

    BoundaryCondition *bc2=new BoundaryCondition;
    bc2->setName("Intérieur (flux horizontal)");
    bc2->setResistance(0.13, 0);
    bc2->setTemperature(20.0);
    bc2->setColor(223, 109, 20);
    bc2->setId("9D7B0B22-6578-42EE-9989-FAF7C0B1B37B");
    _standard.push_back(bc2);

    BoundaryCondition *bc3=new BoundaryCondition;
    bc3->setName("Intérieur (flux horizontal réduit)");
    bc3->setResistance(0.20, 0);
    bc3->setTemperature(20.0);
    bc3->setColor(246, 229, 8);
    bc3->setId("9DRB0B67-6578-42EE-CAF1-FAF7C0B1B37B");
    _standard.push_back(bc3);

    BoundaryCondition *bc4=new BoundaryCondition;
    bc4->setName("Intérieur (flux descendant)");
    bc4->setResistance(0.17, 0);
    bc4->setTemperature(20.0);
    bc4->setColor(225, 206, 154);
    bc4->setId("B33C4F4E-2749-4A14-A022-FCEB5F4A713C");
    _standard.push_back(bc4);

    BoundaryCondition *bc5=new BoundaryCondition;
    bc5->setName("Intérieur (flux ascendant)");
    bc5->setResistance(0.10, 0);
    bc5->setTemperature(20.0);
    bc5->setColor(115, 8, 0);
    bc5->setId("10F097B9-6864-42CC-9E9B-C7BBD07371B0");
    _standard.push_back(bc5);
}

BoundaryConditions::~BoundaryConditions()
{
    for (unsigned int i=0 ; i<_standard.size() ; i++)
        delete _standard.at(i);
    for (unsigned int i=0 ; i<_user.size() ; i++)
        delete _user.at(i);
}

void BoundaryConditions::addBcToUserDb(BoundaryCondition *bc)
{
    _user.push_back(bc);
    boundaryConditionAdded(bc->id());
}

void BoundaryConditions::addBcToStandardDb(BoundaryCondition *bc)
{
    _standard.push_back(bc);
}

BoundaryCondition *BoundaryConditions::bcFromId(const std::string &id) const
{
    for (unsigned int i=0 ; i<_standard.size() ; i++)
        if (_standard.at(i)->id()==id)
            return _standard.at(i);
    for (unsigned int i=0 ; i<_user.size() ; i++)
        if (_user.at(i)->id()==id)
            return _user.at(i);
    return 0;
}

BoundaryCondition *BoundaryConditions::bcFromTemperatureAndResistance(double temperature, double resistance) const
{
    for (unsigned int i=0 ; i<_standard.size() ; i++)
        if (_standard.at(i)->temperature()==temperature && _standard.at(i)->resistance()==resistance)
            return _standard.at(i);
    for (unsigned int i=0 ; i<_user.size() ; i++)
        if (_user.at(i)->temperature()==temperature && _user.at(i)->resistance()==resistance)
            return _user.at(i);
    return 0;
}

BoundaryCondition * BoundaryConditions::duplicate(BoundaryCondition *bc) const
{
    for (unsigned int i=0 ; i<_standard.size() ; i++)
        if (_standard.at(i)->isEquivalentTo(bc))
            return _standard.at(i);
    for (unsigned int i=0 ; i<_user.size() ; i++)
        if (_user.at(i)->isEquivalentTo(bc))
            return _user.at(i);
    return 0;
}

void BoundaryConditions::registerObserver(BoundaryConditionsObserver *observer)
{
    _observers.push_back(observer);
}

void BoundaryConditions::unregisterObserver(BoundaryConditionsObserver *observer)
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

void BoundaryConditions::deleteBoundaryConditionById(const std::string &id)
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
    boundaryConditionDeleted(id);
}

void BoundaryConditions::boundaryConditionDeleted(const std::string &id)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->boundaryConditionDeleted(id);
}

void BoundaryConditions::boundaryConditionAdded(const std::string &id)
{
    if (!_informObservers)
        return;
    for (unsigned int i=0 ; i<_observers.size() ; i++)
        _observers.at(i)->boundaryConditionAdded(id);
}

void BoundaryConditions::informObservers(bool inform)
{
    _informObservers=inform;
}
