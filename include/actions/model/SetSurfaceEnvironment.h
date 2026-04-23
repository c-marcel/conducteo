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

#ifndef _SET_SURFACE_ENVIRONMENT_H
#define _SET_SURFACE_ENVIRONMENT_H

#include <actions/UserAction.h>
#include <string>

class Model;

class SetSurfaceEnvironment: public UserAction
{

public:

    SetSurfaceEnvironment(Model *model, const std::string &environmentName, double x, double y, double precision);
    virtual ~SetSurfaceEnvironment();

    void exec();
    void undo();

private:

    Model              *_model;
    std::string         _environmentName;
    std::string         _oldEnvironmentName;
    double              _x;
    double              _y;
    double              _precision;

};

#endif
