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

#ifndef _BRING_DOWN_VOLUME_H
#define _BRING_DOWN_VOLUME_H

#include <actions/boundary-conditions/StorePhysicAction.h>

class Volume;
class Model;

class BringDownVolume: public StorePhysicAction
{

public:

    BringDownVolume(Model *model, Volume *volume);
    virtual ~BringDownVolume();

    void exec();
    void undo();

private:

    Model      *_model;
    std::string _volumeId;

};

#endif
