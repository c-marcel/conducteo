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

#ifndef _EN_13370_INTERFACE_H
#define _EN_13370_INTERFACE_H

#include <plugins/HeatFlowComputerInterface.h>
#include <plugins/En13370Plugin.h>
#include <Translatable.h>
#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

class ComboBoxMaterialDelegate;

class En13370Interface: public HeatFlowComputerInterface,
                        public Translatable
{

    Q_OBJECT

public:

    En13370Interface(QWidget *parent=0);
    virtual ~En13370Interface();

    void loadPlugin(HeatFlowComputerPlugin *plugin);
    void translate();
    void applyTheme();

    void reloadMaterials();

private slots:

    void typeChanged(int type);
    void updateVisibility();

    void focusChanged(QWidget *old, QWidget *now);
    void dataChanged(const QString &data);
    void updateResults();

private:

    void clean();
    double readDouble(const QString &value) const;
    void invalidateResults();
    void displayResults(double flux);

    double floorWidth() const;
    double groundConductivity() const;
    En13370Plugin::LowFloorType floorType() const;
    double windFactor() const;
    QString toString(double value) const;

    En13370Plugin  *_plugin;

    QGridLayout     _grid;
    QVBoxLayout     _layout;

    QButtonGroup    _group1;
    QButtonGroup    _group2;
    QButtonGroup    _group3;
    QButtonGroup    _group4;

    QHBoxLayout     _globalOptions;
    QVBoxLayout     _floorDimensions;
    QHBoxLayout     _floorWidthLayout;
    QHBoxLayout     _floorAreaAndPerimeterLayout;
    QHBoxLayout     _modelFloorWidthLayout;
    QLabel          _floorDimensionsLabel;
    QRadioButton    _floorWidth;
    QRadioButton    _floorAreaAndPerimeter;
    QLabel          _floorWidthUnit;
    QLabel          _floorAreaUnit;
    QLabel          _floorPerimeter;
    QLabel          _floorPerimeterUnit;
    QLineEdit       _floorWidthValue;
    QLineEdit       _floorAreaValue;
    QLineEdit       _floorPerimeterValue;
    QLabel          _modelFloorWidth;
    QLabel          _modelFloorWidthUnit;
    QLineEdit       _modelFloorWidthValue;

    QVBoxLayout     _groundPropertiesLayout;
    QHBoxLayout     _groundMaterialLayout;
    QHBoxLayout     _groundConductivityLayout;
    QLabel          _groundProperties;
    QRadioButton    _groundMaterial;
    QComboBox       _groundMaterialValue;
    QRadioButton    _groundConductivity;
    QLineEdit       _groundConductivityValue;
    QLabel          _groundConductivityUnit;

    QHBoxLayout     _temperaturesEnvLayout;
    QVBoxLayout     _temperaturesLayout;
    QVBoxLayout     _envLayout;
    QHBoxLayout     _internalTempLayout;
    QHBoxLayout     _externalTempLayout;
    QHBoxLayout     _internalEnvLayout;
    QHBoxLayout     _externalEnvLayout;
    QLabel          _conditionsTitle;
    QLabel          _intTempTitle;
    QLabel          _extTempTitle;
    QLabel          _intTempUnit;
    QLabel          _extTempUnit;
    QLabel          _intEnvTitle;
    QLabel          _extEnvTitle;
    QLineEdit       _intTempValue;
    QLineEdit       _extTempValue;
    QComboBox       _intEnvValue;
    QComboBox       _extEnvValue;

    QHBoxLayout     _typeLayout;
    QLabel          _typeTitle;
    QComboBox       _typeValue;

    QVBoxLayout     _centralLayout;

    QHBoxLayout     _resultsLayout;
    QLabel          _resultsTitle;
    QLabel          _resultsValue;
    QLabel          _resultsUnit;

    QHBoxLayout     _insulationMeanFloor;
    QVBoxLayout     _meanFloorLayout;
    QHBoxLayout     _wallThicknessLayout;
    QHBoxLayout     _groundRsiLayout;
    QHBoxLayout     _groundRLayout;
    QHBoxLayout     _groundRseLayout;
    QLabel          _wallThicknessTitle;
    QLabel          _wallThicknessUnit;
    QLabel          _groundRsiTitle;
    QLabel          _groundRsiUnit;
    QLabel          _groundRTitle;
    QLabel          _groundRUnit;
    QLabel          _groundRseTitle;
    QLabel          _groundRseUnit;
    QLabel          _groundFloorTitle;
    QLineEdit       _wallThicknessValue;
    QLineEdit       _groundRsiValue;
    QLineEdit       _groundRValue;
    QLineEdit       _groundRseValue;

    QVBoxLayout     _insulationLayout;
    QVBoxLayout     _insulationHLayout;
    QVBoxLayout     _insulationVLayout;
    QHBoxLayout     _insulationVThickLayout;
    QHBoxLayout     _insulationHThickLayout;
    QHBoxLayout     _insulationVResLayout;
    QHBoxLayout     _insulationHResLayout;
    QHBoxLayout     _insulationVLenLayout;
    QHBoxLayout     _insulationHLenLayout;
    QLabel          _insulationTitle;
    QLabel          _hInsThickTitle;
    QLabel          _vInsThickTitle;
    QLabel          _hInsThickUnit;
    QLabel          _vInsThickUnit;
    QLabel          _hInsResTitle;
    QLabel          _vInsResTitle;
    QLabel          _hInsResUnit;
    QLabel          _vInsResUnit;
    QLabel          _hInsLenTitle;
    QLabel          _vInsLenTitle;
    QLabel          _hInsLenUnit;
    QLabel          _vInsLenUnit;
    QLineEdit       _hInsThickValue;
    QLineEdit       _vInsThickValue;
    QLineEdit       _hInsResValue;
    QLineEdit       _vInsResValue;
    QLineEdit       _hInsLenValue;
    QLineEdit       _vInsLenValue;
    QCheckBox       _enableHInsulation;
    QCheckBox       _enableVInsulation;

    QWidget         _crawlWidget;
    QVBoxLayout     _crawlLayout;
    QHBoxLayout     _crawlLayoutItem1;
    QHBoxLayout     _crawlLayoutItem2;
    QHBoxLayout     _crawlLayoutItem3;
    QHBoxLayout     _crawlLayoutItem4;
    QHBoxLayout     _crawlLayoutItem5;
    QHBoxLayout     _crawlLayoutItem6;
    QHBoxLayout     _crawlLayoutItem7;
    QHBoxLayout     _crawlLayoutItem8;
    QHBoxLayout     _crawlLayoutItem9;
    QHBoxLayout     _crawlLayoutItem10;
    QVBoxLayout     _windLayout;
    QHBoxLayout     _wind1Layout;
    QHBoxLayout     _wind2Layout;
    QLabel          _crawlTitle1;
    QLabel          _crawlTitle2;
    QLabel          _crawlTitle3;
    QLabel          _crawlTitle4;
    QLabel          _crawlTitle5;
    QLabel          _crawlTitle6;
    QLabel          _crawlTitle7;
    QLabel          _crawlTitle8;
    QLabel          _crawlTitle9;
    QLabel          _crawlTitle10;
    QLabel          _crawlUnit1;
    QLabel          _crawlUnit2;
    QLabel          _crawlUnit3;
    QLabel          _crawlUnit4;
    QLabel          _crawlUnit6;
    QLabel          _crawlUnit7;
    QLabel          _crawlUnit8;
    QLabel          _crawlUnit9;
    QLabel          _crawlUnit10;
    QLineEdit       _crawlValue1;
    QLineEdit       _crawlValue2;
    QLineEdit       _crawlValue3;
    QLineEdit       _crawlValue4;
    QLineEdit       _crawlValue6;
    QLineEdit       _crawlValue7;
    QLineEdit       _crawlValue8;
    QLineEdit       _crawlValue9;
    QLineEdit       _crawlValue10;
    QRadioButton    _defaultWindTitle;
    QRadioButton    _userWindTitle;
    QComboBox       _defaultWindValue;
    QLineEdit       _userWindValue;

    QWidget         _basementWidget;
    QVBoxLayout     _basementLayout;
    QHBoxLayout     _basementLayoutCommon;
    QVBoxLayout     _basementLayoutCommon1;
    QVBoxLayout     _basementLayoutCommon2;
    QHBoxLayout     _basementLayoutItem1;
    QHBoxLayout     _basementLayoutItem2;
    QHBoxLayout     _basementLayoutItem3;
    QHBoxLayout     _basementLayoutItem4;
    QHBoxLayout     _basementLayoutItem5;
    QHBoxLayout     _basementLayoutItem6;
    QHBoxLayout     _basementLayoutItem7;
    QHBoxLayout     _basementLayoutItem10;
    QHBoxLayout     _basementLayoutItem11;
    QHBoxLayout     _basementLayoutItem8;
    QHBoxLayout     _basementLayoutItem9;
    QLabel          _basementTitle1;
    QLabel          _basementTitle2;
    QLabel          _basementTitle3;
    QLabel          _basementTitle4;
    QLabel          _basementTitle5;
    QLabel          _basementTitle6;
    QLabel          _basementTitle7;
    QLabel          _basementTitle8;
    QLabel          _basementTitle9;
    QLabel          _basementTitle10;
    QLabel          _basementTitle11;
    QLabel          _basementUnit1;
    QLabel          _basementUnit2;
    QLabel          _basementUnit3;
    QLabel          _basementUnit4;
    QLabel          _basementUnit5;
    QLabel          _basementUnit6;
    QLabel          _basementUnit7;
    QLabel          _basementUnit8;
    QLabel          _basementUnit9;
    QLabel          _basementUnit10;
    QLabel          _basementUnit11;
    QLineEdit       _basementValue1;
    QLineEdit       _basementValue2;
    QLineEdit       _basementValue3;
    QLineEdit       _basementValue4;
    QLineEdit       _basementValue5;
    QLineEdit       _basementValue6;
    QLineEdit       _basementValue7;
    QLineEdit       _basementValue10;
    QLineEdit       _basementValue11;
    QLineEdit       _basementValue8;
    QLineEdit       _basementValue9;
    QLabel          _wallBasementLabel;
    QLabel          _basementHeatingType;
    QHBoxLayout     _basementHeatingLayout1;
    QHBoxLayout     _basementHeatingLayout2;
    QHBoxLayout     _basementHeatingLayout3;
    QRadioButton    _basementHeating1;
    QRadioButton    _basementHeating2;
    QRadioButton    _basementHeating3;
    QLabel          _basementExtWallTitle;

    QLabel          _helpImage;
    QPixmap         _emptyPixmap;
    QPixmap         _13370_basement_ceil_resistance;
    QPixmap         _13370_basement_external_wall;
    QPixmap         _13370_basement_external_wall_height;
    QPixmap         _13370_basement_height;
    QPixmap         _13370_basement_mean_height;
    QPixmap         _13370_basement_wall;
    QPixmap         _13370_crawl_depth;
    QPixmap         _13370_crawl_height;
    QPixmap         _13370_crawl_wall;
    QPixmap         _13370_external;
    QPixmap         _13370_floor_area;
    QPixmap         _13370_floor_perimeter;
    QPixmap         _13370_floor_resistance_1;
    QPixmap         _13370_floor_resistance_2;
    QPixmap         _13370_floor_resistance_3;
    QPixmap         _13370_floor_width;
    QPixmap         _13370_ground_conductivity;
    QPixmap         _13370_internal;
    QPixmap         _13370_model_floor_width;
    QPixmap         _13370_wall_thickness_1;
    QPixmap         _13370_wall_thickness_2;
    QPixmap         _13370_wall_thickness_3;
    QPixmap         _13370_crawl_wall_ground;
    QPixmap         _13370_iso_h_resistance;
    QPixmap         _13370_iso_h_thickness;
    QPixmap         _13370_iso_h_width;
    QPixmap         _13370_iso_v_height;
    QPixmap         _13370_iso_v_resistance;
    QPixmap         _13370_iso_v_thickness;

    bool            _importData;

    ComboBoxMaterialDelegate   *_delegate;

};

#endif
