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

#ifndef _HEAT_FLOW_COMPUTER_PLUGIN_H
#define _HEAT_FLOW_COMPUTER_PLUGIN_H

#include <string>

class Model;

class HeatFlowComputerPlugin
{

public:

    HeatFlowComputerPlugin();
    virtual ~HeatFlowComputerPlugin();

    void setModel(Model *model);
    virtual std::string name() const=0;

    virtual void fromXml(const std::string &xml)=0;
    virtual std::string toXml() const=0;

    virtual double flux() const=0;
    virtual double flux(const std::string &environment1, const std::string &environment2) const=0;

protected:

    Model  *_model;

};

#endif
