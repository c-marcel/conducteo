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

#ifndef _COPY_2D_MODEL_TO_1D_MODEL
#define _COPY_2D_MODEL_TO_1D_MODEL

#include <actions/UserAction.h>
#include <model/Model.h>

class Model;

class Copy2dModelTo1dModel: public UserAction
{

public:

    Copy2dModelTo1dModel(Model *model);
    virtual ~Copy2dModelTo1dModel();

    void exec();
    void undo();

private:

    Model      *_model;
    Model      *_1dModel;

};

#endif
