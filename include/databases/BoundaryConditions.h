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

#ifndef BOUNDARY_CONDITIONS_H
#define BOUNDARY_CONDITIONS_H

#include <tools/Singleton.h>
#include <vector>
#include <string>

class BoundaryConditionsObserver;
class BoundaryCondition;

class BoundaryConditions: public Singleton<BoundaryConditions>
{

public:

    BoundaryConditions();
    virtual ~BoundaryConditions();

    void informObservers(bool inform);
    void registerObserver(BoundaryConditionsObserver *observer);
    void unregisterObserver(BoundaryConditionsObserver *observer);

    void addBcToUserDb(BoundaryCondition *bc);
    void addBcToStandardDb(BoundaryCondition *bc);

    void deleteBoundaryConditionById(const std::string &id);

    BoundaryCondition *bcFromId(const std::string &id) const;
    BoundaryCondition *bcFromTemperatureAndResistance(double temperature, double resistance) const;
    BoundaryCondition *duplicate(BoundaryCondition *bc) const;

private:

    void boundaryConditionDeleted(const std::string &id);
    void boundaryConditionAdded(const std::string &id);

    std::vector<BoundaryCondition*>             _standard;
    std::vector<BoundaryCondition*>             _user;
    std::vector<BoundaryConditionsObserver*>    _observers;
    
    bool                                        _informObservers;

};

#endif
