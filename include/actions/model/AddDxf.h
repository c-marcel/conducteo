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

#ifndef _ADD_DXF_H
#define _ADD_DXF_H

#include <actions/UserAction.h>
#include <string>

class Model;
class DxfContent;

class AddDxf: public UserAction
{

public:

    AddDxf(Model *model, const std::string &data);
    virtual ~AddDxf();

    void exec();
    void undo();

    DxfContent *dxf() const;

private:

    Model      *_model;
    std::string _dxfId;

    std::string _data;

};

#endif
