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

#ifndef _SET_SURFACE_ENVIRONMENT_FROM_SELECTION_H
#define _SET_SURFACE_ENVIRONMENT_FROM_SELECTION_H

#include <actions/boundary-conditions/StorePhysicAction.h>
#include <string>

class Surface;
class Model;

class SetEnvironmentFromSelection: public StorePhysicAction
{
public:
    SetEnvironmentFromSelection(Model *model, const std::string &environmentName, double x, double y, double width, double height);
    virtual ~SetEnvironmentFromSelection();

    void exec();
    void undo();

    bool isValid() const;

private:
    std::vector<Surface*>   availableSurfaces() const;

    Model      *_model;
    double      _x;
    double      _y;
    double      _width;
    double      _height;
    std::string _environment;
};

#endif
