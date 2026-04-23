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

#ifndef _EN_13370_PLUGIN_H
#define _EN_13370_PLUGIN_H

#include <plugins/HeatFlowComputerPlugin.h>

class En13370Plugin: public HeatFlowComputerPlugin
{

public:

    enum LowFloorType
    {
        Median,
        CrawlSpace,
        HeatedUnderground,
        NotHeatedUnderground,
        PartiallyHeatedUnderground
    };

    En13370Plugin();
    virtual ~En13370Plugin();

    std::string name() const;

    void fromXml(const std::string &xml);
    std::string toXml() const;

    void setType(LowFloorType type);
    LowFloorType type() const;

    void includeHorizontalInsulation(bool include);
    bool includeHorizontalInsulation() const;

    void includeVerticalInsulation(bool include);
    bool includeVerticalInsulation() const;

    void setFloorWidth(double width);
    double floorWidth() const;

    void setModelFloorWidth(double width);
    double modelFloorWidth() const;

    void setGroundConductivity(double conductivity);
    double groundConductivity() const;

    void setInternalTemperature(double temperature);
    double internalTemperature() const;

    void setExternalTemperature(double temperature);
    double externalTemperature() const;

    void setInternalEnvironment(const std::string &environment);
    std::string internalEnvironment() const;

    void setExternalEnvironment(const std::string &environment);
    std::string externalEnvironment() const;

    double flux() const;
    double flux(const std::string &environment1, const std::string &environment2) const;

    void setWallThickness(double thickness);
    double wallThickness() const;

    void setFloorResistance(double resistance);
    double floorResistance() const;

    void setFloorInternalSuperficialResistance(double resistance);
    double floorInternalSuperficialResistance() const;

    void setFloorExternalSuperficialResistance(double resistance);
    double floorExternalSuperficialResistance() const;

    void setUndergroundDepth(double depth);
    double undergroundDepth() const;

    void setWallInternalSuperficialResistance(double resistance);
    double wallInternalSuperficialResistance() const;

    void setWallExternalSuperficialResistance(double resistance);
    double wallExternalSuperficialResistance() const;

    void setWallResistance(double resistance);
    double wallResistance() const;

    void setHeatedUndergroundArea(double area);
    double heatedUndergroundArea() const;

    void setNotHeatedUndergroundArea(double area);
    double notHeatedUndergroundArea() const;

    void setExternalWallHeight(double height);
    double externalWellHeight() const;

    void setCrawlHeight(double height);
    double crawlHeight() const;

    void setExternalWallResistance(double resistance);
    double externalWallResistance() const;

    void setMeanZoneHeight(double height);
    double meanZoneHeight() const;

    void setAirExchangeRate(double rate);
    double airExchangeRate() const;

    void setZoneCeilResistance(double resistance);
    double zoneCeilResistance() const;

    void setOpeningsRatio(double ratio);
    double openingsRatio() const;

    void setWindSpeed(double speed);
    double windSpeed() const;

    void setWindProtectionFactor(double factor);
    double windProtectionFactor() const;

    void setInsulationWidth(double width);
    double insulationWidth() const;

    void setInsulationHeight(double height);
    double insulationHeight() const;

    void setHorizontalInsulationResistance(double resistance);
    double horizontalInsulationResistance() const;

    void setVerticalInsulationResistance(double resistance);
    double verticalInsulationResistance() const;

    void setHorizontalInsulationThickness(double thickness);
    double horizontalInsulationThickness() const;

    void setVerticalInsulationThickness(double thickness);
    double verticalInsulationThickness() const;

    void useFloorWidth(bool use);
    void setPerimeter(double perimeter);
    void setArea(double area);

    bool useFloorWidth() const;
    double perimeter() const;
    double area() const;

    void useConductivity(bool use);
    void setMaterialId(const std::string &id);

    bool useConductivity() const;
    std::string materialId() const;

private:

    double getU0() const;
    double getDeltaUPsi() const;
    double getDeltaTemperature() const;

    double getMedianU0() const;
    double getCrawlSpaceU0() const;
    double getHeatedUndergroundU0() const;
    double getNotHeatedUndergroundU0() const;
    double getPartiallyHeatedUndergroundU0() const;

    double getHorizontalInsulationU() const;
    double getVerticalInsulationU() const;

    double getVerticalHeight() const;

    LowFloorType    _type;
    bool            _includeHorizontalInsulation;
    bool            _includeVerticalInsulation;
    double          _floorWidth;
    double          _area;
    double          _perimeter;
    bool            _useFloorWidth;
    double          _modelFloorWidth;
    double          _groundConductivity;
    std::string     _materialId;
    bool            _useConductivity;
    double          _internalTemperature;
    double          _externalTemperature;
    std::string     _internalEnvironment;
    std::string     _externalEnvironment;

    double          _wallThickness;
    double          _floorThermalResistance;
    double          _floorInternalSuperficialResistance;
    double          _floorExternalSuperficialResistance;

    double          _underGroundDepth;
    double          _wallInternalSuperficialResistance;
    double          _wallExternalSuperficialResistance;
    double          _wallThermalResistance;

    double          _heatedUndergroundArea;
    double          _notHeatedUndergroundArea;

    double          _crawlHeight;
    double          _externalWallHeight;
    double          _externalWallResistance;
    double          _meanZoneHeight;
    double          _airExchangeRate;
    double          _zoneCeilResistance;

    double          _openingsRatio;
    double          _windSpeed;
    double          _windProtectionFactor;

    double          _insulationWidth;
    double          _insulationHeight;
    double          _horizontalInsulationResistance;
    double          _verticalInsulationResistance;
    double          _horizontalInsulationThickness;
    double          _verticalInsulationThickness;

};

#endif
