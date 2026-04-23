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

#ifndef _ADD_POLYLINE_H
#define _ADD_POLYLINE_H

#include <actions/boundary-conditions/StorePhysicAction.h>
#include <geometry/Point.h>
#include <vector>
#include <string>

class Model;

class AddPolyline: public StorePhysicAction
{

public:

    AddPolyline(Model *model, const std::vector<Point> &points, const std::string &materialId);
    virtual ~AddPolyline();

    void exec();
    void undo();

private:

    Model              *_model;
    std::string         _materialId;
    std::string         _volumeId;
    std::vector<Point>  _points;

};

#endif
