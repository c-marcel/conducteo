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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <actions/Observable.h>
#include <string>

class ErrorHandler;

class Material: public Observable
{

public:

    Material();
    Material(const Material &material);
    virtual ~Material();

    bool check(ErrorHandler *error);

    void setName(const std::string &name);
    void setDescription(const std::string &description);
    void setId(const std::string &id);
    void setConductivity(double conductivity, ErrorHandler *error);
    void setColor(unsigned int red, unsigned int green, unsigned int blue);
    void setEmissivity(double emissivity, ErrorHandler *error);
    void setMu(double mu, ErrorHandler *error);
    void setFavorite(bool favorite);
    void setCategory(const std::string &category);

    std::string name() const;
    std::string description() const;
    std::string id() const;
    double conductivity() const;
    double mu() const;
    unsigned int red() const;
    unsigned int green() const;
    unsigned int blue() const;
    double emissivity() const;
    bool favorite() const;
    std::string category() const;

    std::string serialize() const;

    bool isEqualTo(const Material *material) const;
    bool isEquivalentTo(const Material *material) const;

    static Material *createFromXml(const std::string &xml, ErrorHandler *error);

private:

    std::string     _name;
    std::string     _description;
    std::string     _id;
    std::string     _category;
    double          _conductivity;
    double          _emissivity;
    double          _mu;
    unsigned int    _red;
    unsigned int    _green;
    unsigned int    _blue;
    bool            _favorite;

};

#endif
