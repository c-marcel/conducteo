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

#ifndef _REGULAR_AIR_CAVITY_H
#define _REGULAR_AIR_CAVITY_H

class RegularAirCavity
{

public:

    enum Type
    {
        Unventilated,
        PartiallyVentilated
    };

    RegularAirCavity();
    virtual ~RegularAirCavity();

    void setType(Type type);
    void setHeight(double height);
    void setWidth(double width);
    void setLeftEmissivity(double emissivity);
    void setRightEmissivity(double emissivity);
    void setTopEmissivity(double emissivity);
    void setBottomEmissivity(double emissivity);
    void setMeanTemperature(double temperature);
    void setMaxTemperatureDifference(double temperature);

    void computeEquivalenteConductivities();
    double horizontalConductivity() const;
    double verticalConductivity() const;

    Type type() const;

private:

    Type    _type;

    double  _height;
    double  _width;
    double  _leftEmissivity;
    double  _rightEmissivity;
    double  _topEmissivity;
    double  _bottomEmissivity;

    double  _meanTemperature;
    double  _maxTemperatureDifference;

    double  _horizontalConductivity;
    double  _verticalConductivity;

};

#endif
