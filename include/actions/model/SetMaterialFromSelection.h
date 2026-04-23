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

#ifndef _SET_MATERIAL_FROM_SELECTION_H
#define _SET_MATERIAL_FROM_SELECTION_H

#include <actions/UserAction.h>
#include <vector>
#include <string>
#include <map>

class Volume;
class Model;

class SetMaterialFromSelection: public UserAction
{
public:
    SetMaterialFromSelection(Model *model, const std::string &id, double x, double y, double width, double height);
    virtual ~SetMaterialFromSelection();

    void exec();
    void undo();

    bool isValid() const;

private:
    void storePhysics();
    void restorePhysics();
    std::vector<Volume*> availableVolumes() const;

    Model                              *_model;
    double                              _x;
    double                              _y;
    double                              _width;
    double                              _height;
    std::string                         _id;
    std::map<std::string, std::string>  _storedData;
};

#endif
