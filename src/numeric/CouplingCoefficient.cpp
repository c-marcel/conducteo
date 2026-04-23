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

#include <numeric/CouplingCoefficient.h>

CouplingCoefficient::CouplingCoefficient():
    _couplingCoefficient(0.0),
    _type(CouplingCoefficient2d)
{
}

void CouplingCoefficient::setType(Type type)
{
    _type=type;
}
    
void CouplingCoefficient::setFirstEnvironment(const std::string &id)
{
    _environment1Id=id;
}

void CouplingCoefficient::setSecondEnvironment(const std::string &id)
{
    _environment2Id=id;
}

void CouplingCoefficient::setCouplingCoefficient(double coefficient)
{
    _couplingCoefficient=coefficient;
}

CouplingCoefficient::Type CouplingCoefficient::type() const
{
    return _type;
}

std::string CouplingCoefficient::firstEnvironmentId() const
{
    return _environment1Id;
}

std::string CouplingCoefficient::secondEnvironmentId() const
{
    return _environment2Id;
}

double CouplingCoefficient::couplingCoefficient() const
{
    return _couplingCoefficient;
}
