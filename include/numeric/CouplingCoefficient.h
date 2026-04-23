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

#ifndef _COUPLING_COEFFICIENT_H
#define _COUPLING_COEFFICIENT_H

#include <string>

class CouplingCoefficient
{

public:

    enum Type
    {
        CouplingCoefficient2d,
        CouplingCoefficient1d
    };

    CouplingCoefficient();
    
    void setType(Type type);
    void setFirstEnvironment(const std::string &id);
    void setSecondEnvironment(const std::string &id);
    void setCouplingCoefficient(double coefficient);

    Type type() const;
    std::string firstEnvironmentId() const;
    std::string secondEnvironmentId() const;
    double couplingCoefficient() const;

private:

    Type        _type;
    std::string _environment1Id;
    std::string _environment2Id;
    double      _couplingCoefficient;

};

#endif
