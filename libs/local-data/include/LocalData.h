// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2019, Clément MARCEL.
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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <tools/Singleton.h>
#include <string>
#include <map>

class LocalData: public Singleton<LocalData>
{

public:

    LocalData();
    virtual ~LocalData();

    void setParameter(const std::string &parameter, const std::string &value);
    std::string getParameter(const std::string &parameter);

    void save();
    void load();
    void enableAutoSave(const bool autosave=true);
    std::string getLocalPath() const;

private:

    void createLocalPath();

    std::string                         _path;
    std::map<std::string, std::string>  _parameters;
    bool                                _autosave;

};

#endif