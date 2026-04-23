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

#include <physics/Environment.h>
#include <errors/ErrorHandler.h>

Environment::Environment():
    _red(0),
    _green(0),
    _blue(0)
{
}

Environment::~Environment()
{
}

bool Environment::check(ErrorHandler *error)
{
    if (_name.empty())
    {
        setError(error, BadEnvironmentName);
        return false;
    }

    if (_red>255)
    {
        setError(error, BadEnvironmentColor);
        return false;
    }

    if (_green>255)
    {
        setError(error, BadEnvironmentColor);
        return false;
    }

    if (_blue>255)
    {
        setError(error, BadEnvironmentColor);
        return false;
    }

    return true;
}

void Environment::setName(const std::string &name)
{
    _name=name;
}

void Environment::setDescription(const std::string &description)
{
    _description=description;
}

void Environment::setColor(unsigned int red, unsigned int green, unsigned int blue)
{
    _red=red;
    _green=green;
    _blue=blue;
}

std::string Environment::name() const
{
    return _name;
}

std::string Environment::description() const
{
    return _description;
}

unsigned int Environment::red() const
{
    return _red;
}

unsigned int Environment::green() const
{
    return _green;
}

unsigned int Environment::blue() const
{
    return _blue;
}
