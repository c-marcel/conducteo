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

#include <physics/RegularAirCavity.h>
#include <algorithm>
#include <math.h>

RegularAirCavity::RegularAirCavity():
    _type(Unventilated),
    _height(0.0),
    _width(0.0),
    _leftEmissivity(0.9),
    _rightEmissivity(0.9),
    _topEmissivity(0.9),
    _bottomEmissivity(0.9),
    _meanTemperature(283.0),
    _maxTemperatureDifference(10.0)
{
}

RegularAirCavity::~RegularAirCavity()
{
}

void RegularAirCavity::setType(Type type)
{
    _type=type;
}

void RegularAirCavity::setHeight(double height)
{
    _height=height;
}

void RegularAirCavity::setWidth(double width)
{
    _width=width;
}

void RegularAirCavity::setLeftEmissivity(double emissivity)
{
    _leftEmissivity=emissivity;
}

void RegularAirCavity::setRightEmissivity(double emissivity)
{
    _rightEmissivity=emissivity;
}

void RegularAirCavity::setTopEmissivity(double emissivity)
{
    _topEmissivity=emissivity;
}

void RegularAirCavity::setBottomEmissivity(double emissivity)
{
    _bottomEmissivity=emissivity;
}

void RegularAirCavity::setMeanTemperature(double temperature)
{
    _meanTemperature=temperature;
}

void RegularAirCavity::setMaxTemperatureDifference(double temperature)
{
    _maxTemperatureDifference=temperature;
}

void RegularAirCavity::computeEquivalenteConductivities()
{
    // Compute vertical conductivity.
    double ha=0.025/_height;
    if (_width>=0.005)
        ha=std::max(0.025/_height, 0.73*pow(_maxTemperatureDifference, 1.0/3.0));

    double E=1.0/(1.0/_topEmissivity+1.0/_bottomEmissivity-1.0);
    double F=0.5*(1.0-_height/_width+sqrt(1.0+(_height*_height)/(_width*_width)));
    double hr=4.0*5.67e-8*pow(_meanTemperature, 3.0)*E*F;

    double Rs=1.0/(ha+hr);
    _verticalConductivity=_height/Rs;

    if (_type==PartiallyVentilated)
        _verticalConductivity=2.0*_verticalConductivity;

    // Compute horizontal conductivity.
    ha=0.025/_width;
    if (_height>=0.005)
        ha=std::max(0.025/_width, 0.73*pow(_maxTemperatureDifference, 1.0/3.0));

    E=1.0/(1.0/_leftEmissivity+1.0/_rightEmissivity-1.0);
    F=0.5*(1.0-_width/_height+sqrt(1.0+(_width*_width)/(_height*_height)));
    hr=4.0*5.67e-8*pow(_meanTemperature, 3.0)*E*F;

    Rs=1.0/(ha+hr);
    _horizontalConductivity=_width/Rs;

    if (_type==PartiallyVentilated)
        _horizontalConductivity=2.0*_horizontalConductivity;
}

double RegularAirCavity::horizontalConductivity() const
{
    return _horizontalConductivity;
}

double RegularAirCavity::verticalConductivity() const
{
    return _verticalConductivity;
}

RegularAirCavity::Type RegularAirCavity::type() const
{
    return _type;
}
