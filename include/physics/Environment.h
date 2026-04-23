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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>

class ErrorHandler;

class Environment
{

public:

    Environment();
    virtual ~Environment();

    bool check(ErrorHandler *error);

    void setName(const std::string &name);
    void setDescription(const std::string &description);
    void setColor(unsigned int red, unsigned int green, unsigned int blue);

    std::string name() const;
    std::string description() const;
    unsigned int red() const;
    unsigned int green() const;
    unsigned int blue() const;

private:

    std::string     _name;
    std::string     _description;
    unsigned int    _red;
    unsigned int    _green;
    unsigned int    _blue;

};

#endif
