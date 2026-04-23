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

#ifndef BOUNDARY_CONDITION_H
#define BOUNDARY_CONDITION_H

#include <actions/Observable.h>
#include <string>

class ErrorHandler;

class BoundaryCondition: public Observable
{

public:

    BoundaryCondition();
    virtual ~BoundaryCondition();

    bool check(ErrorHandler *error);

    void setName(const std::string &name);
    void setDescription(const std::string &description);
    void setId(const std::string &id);
    void setResistance(double resistance, ErrorHandler *error);
    void setTemperature(double temperature);
    void setColor(unsigned int red, unsigned int green, unsigned int blue);
    void setRh(double rh, ErrorHandler *error);

    std::string name() const;
    std::string description() const;
    std::string id() const;
    double resistance() const;
    double temperature() const;
    unsigned int red() const;
    unsigned int green() const;
    unsigned int blue() const;
    double rh() const;

    std::string serialize() const;

    bool isEquivalentTo(const BoundaryCondition *boundary) const;
    bool isEqualTo(const BoundaryCondition *boundary) const;

    static BoundaryCondition *createFromXml(const std::string &xml, ErrorHandler *error);

private:

    std::string     _name;
    std::string     _description;
    std::string     _id;
    double          _resistance;
    double          _temperature;
    double          _rh;
    unsigned int    _red;
    unsigned int    _green;
    unsigned int    _blue;

};

extern double InfiniteResistance;

#endif
