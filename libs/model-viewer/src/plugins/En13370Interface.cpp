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

#include <plugins/En13370Interface.h>
#include <ComboBoxMaterialDelegate.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <LinguistManager.h>
#include <QApplication>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#endif

#ifdef __linux__
using namespace std;
#endif

En13370Interface::En13370Interface(QWidget *parent):
    HeatFlowComputerInterface(parent),
    _grid(this),
    _crawlLayout(&_crawlWidget),
    _basementLayout(&_basementWidget),
    _plugin(0),
    _importData(false),
    _delegate(0)
{
    setFixedWidth(850);
#ifdef __APPLE__
	setFixedHeight(850);
#else
    setFixedHeight(650);
#endif

    QFont f=font();
    f.setUnderline(true);

    _floorDimensionsLabel.setFont(f);
    _floorDimensions.addWidget(&_floorDimensionsLabel);

    _group1.addButton(&_floorWidth);
    _group1.addButton(&_floorAreaAndPerimeter);

    _floorWidthValue.setFixedWidth(50);
    _floorWidth.setFixedWidth(70);
    _floorWidth.setChecked(true);
    _floorWidthLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _floorWidthLayout.addWidget(&_floorWidth);
    _floorWidthLayout.addWidget(&_floorWidthValue);
    _floorWidthLayout.addWidget(&_floorWidthUnit);
    _floorWidthLayout.addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _floorDimensions.addLayout(&_floorWidthLayout);

    _floorAreaValue.setFixedWidth(50);
    _floorPerimeterValue.setFixedWidth(50);
    _floorAreaAndPerimeter.setFixedWidth(70);
    _floorAreaAndPerimeterLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _floorAreaAndPerimeterLayout.addWidget(&_floorAreaAndPerimeter);
    _floorAreaAndPerimeterLayout.addWidget(&_floorAreaValue);
    _floorAreaAndPerimeterLayout.addWidget(&_floorAreaUnit);
    _floorAreaAndPerimeterLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _floorAreaAndPerimeterLayout.addWidget(&_floorPerimeter);
    _floorAreaAndPerimeterLayout.addWidget(&_floorPerimeterValue);
    _floorAreaAndPerimeterLayout.addWidget(&_floorPerimeterUnit);
    _floorAreaAndPerimeterLayout.addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _floorDimensions.addLayout(&_floorAreaAndPerimeterLayout);

    _modelFloorWidthValue.setFixedWidth(50);
    _modelFloorWidthLayout.addWidget(&_modelFloorWidth);
    _modelFloorWidthLayout.addWidget(&_modelFloorWidthValue);
    _modelFloorWidthLayout.addWidget(&_modelFloorWidthUnit);
    _modelFloorWidthLayout.addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _floorDimensions.addLayout(&_modelFloorWidthLayout);
    _floorDimensions.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _globalOptions.addLayout(&_floorDimensions);

    _floorWidthUnit.setText("m");
    _floorAreaUnit.setText("m²");
    _floorPerimeterUnit.setText("m");
    _modelFloorWidthUnit.setText("m");

    _groundProperties.setFont(f);
    _groundPropertiesLayout.addWidget(&_groundProperties);

    _groundMaterial.setFixedWidth(150);
    _groundMaterialValue.setFixedWidth(250);

    _group2.addButton(&_groundMaterial);
    _group2.addButton(&_groundConductivity);
    _groundMaterial.setChecked(true);

    _delegate=new ComboBoxMaterialDelegate(&_groundMaterialValue, this);
    _delegate->changeVolumeMaterial(false);
    _groundMaterialValue.setItemDelegate(_delegate);
    _groundMaterialLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _groundMaterialLayout.addWidget(&_groundMaterial);
    _groundMaterialLayout.addWidget(&_groundMaterialValue);
    _groundMaterialLayout.addItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _groundPropertiesLayout.addLayout(&_groundMaterialLayout);

    _groundConductivity.setFixedWidth(150);
    _groundConductivityValue.setFixedWidth(50);
    _groundConductivityLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _groundConductivityLayout.addWidget(&_groundConductivity);
    _groundConductivityLayout.addWidget(&_groundConductivityValue);
    _groundConductivityLayout.addWidget(&_groundConductivityUnit);
    _groundConductivityLayout.addItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _groundPropertiesLayout.addLayout(&_groundConductivityLayout);

    _groundPropertiesLayout.addLayout(&_insulationLayout);

    _groundPropertiesLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _groundConductivityUnit.setText("W/(m.°C)");

    _globalOptions.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _globalOptions.addLayout(&_groundPropertiesLayout);

    _globalOptions.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _layout.addLayout(&_globalOptions);

    _conditionsTitle.setFont(f);
    _floorDimensions.addWidget(&_conditionsTitle);

    _intTempUnit.setText("°C");
    _extTempUnit.setText("°C");

    _internalTempLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _intTempTitle.setFixedWidth(150);
    _intTempValue.setFixedWidth(50);
    _internalTempLayout.addWidget(&_intTempTitle);
    _internalTempLayout.addWidget(&_intTempValue);
    _internalTempLayout.addWidget(&_intTempUnit);
    _temperaturesLayout.addLayout(&_internalTempLayout);

    _externalTempLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _extTempTitle.setFixedWidth(150);
    _extTempValue.setFixedWidth(50);
    _externalTempLayout.addWidget(&_extTempTitle);
    _externalTempLayout.addWidget(&_extTempValue);
    _externalTempLayout.addWidget(&_extTempUnit);
    _temperaturesLayout.addLayout(&_externalTempLayout);
    _temperaturesLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _temperaturesEnvLayout.addLayout(&_temperaturesLayout);

    _intEnvTitle.setFixedWidth(180);
    _intEnvValue.setFixedWidth(130);
    _internalEnvLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _internalEnvLayout.addWidget(&_intEnvTitle);
    _internalEnvLayout.addWidget(&_intEnvValue);
    _envLayout.addLayout(&_internalEnvLayout);

    _extEnvTitle.setFixedWidth(180);
    _extEnvValue.setFixedWidth(130);
    _externalEnvLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _externalEnvLayout.addWidget(&_extEnvTitle);
    _externalEnvLayout.addWidget(&_extEnvValue);
    _envLayout.addLayout(&_externalEnvLayout);
    _envLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    // _temperaturesEnvLayout.addLayout(&_envLayout);
    _temperaturesEnvLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _floorDimensions.addLayout(&_temperaturesEnvLayout);

    _typeValue.setFixedWidth(170);
    _typeTitle.setFont(f);
    _typeLayout.addWidget(&_typeTitle);
    _typeLayout.addWidget(&_typeValue);
    _typeLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _floorDimensions.addLayout(&_typeLayout);

    _wallThicknessUnit.setText("mm");
    _wallThicknessValue.setFixedWidth(50);
    _wallThicknessTitle.setMinimumWidth(150);
    _wallThicknessLayout.addWidget(&_wallThicknessTitle);
    _wallThicknessLayout.addWidget(&_wallThicknessValue);
    _wallThicknessLayout.addWidget(&_wallThicknessUnit);
    _wallThicknessLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _meanFloorLayout.addLayout(&_wallThicknessLayout);

    _groundFloorTitle.setFont(f);
    _meanFloorLayout.addWidget(&_groundFloorTitle);

    _groundRsiUnit.setText("m².°C/W");
    _groundRsiValue.setFixedWidth(50);
    _groundRsiTitle.setMinimumWidth(200);
    _groundRsiLayout.addWidget(&_groundRsiTitle);
    _groundRsiLayout.addWidget(&_groundRsiValue);
    _groundRsiLayout.addWidget(&_groundRsiUnit);
    _groundRsiLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _meanFloorLayout.addLayout(&_groundRsiLayout);

    _groundRUnit.setText("m².°C/W");
    _groundRValue.setFixedWidth(50);
    _groundRTitle.setMinimumWidth(200);
    _groundRLayout.addWidget(&_groundRTitle);
    _groundRLayout.addWidget(&_groundRValue);
    _groundRLayout.addWidget(&_groundRUnit);
    _groundRLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _meanFloorLayout.addLayout(&_groundRLayout);

    _groundRseUnit.setText("m².°C/W");
    _groundRseValue.setFixedWidth(50);
    _groundRseTitle.setMinimumWidth(200);
    _groundRseLayout.addWidget(&_groundRseTitle);
    _groundRseLayout.addWidget(&_groundRseValue);
    _groundRseLayout.addWidget(&_groundRseUnit);
    _groundRseLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _meanFloorLayout.addLayout(&_groundRseLayout);

    _insulationMeanFloor.addLayout(&_meanFloorLayout);

    _insulationLayout.addWidget(&_insulationTitle);
    _insulationLayout.addLayout(&_insulationHLayout);
    _insulationLayout.addLayout(&_insulationVLayout);

    _insulationTitle.setFont(f);
    _hInsThickUnit.setText("mm");
    _vInsThickUnit.setText("mm");
    _hInsResUnit.setText("m².°C/W");
    _vInsResUnit.setText("m².°C/W");
    _hInsLenUnit.setText("m");
    _vInsLenUnit.setText("m");

    _insulationHLayout.addWidget(&_enableHInsulation);
    _insulationHLayout.addLayout(&_insulationHThickLayout);
    _insulationHLayout.addLayout(&_insulationHResLayout);
    _insulationHLayout.addLayout(&_insulationHLenLayout);

    _hInsThickTitle.setMinimumWidth(200);
    _hInsThickValue.setFixedWidth(50);
    _insulationHThickLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _insulationHThickLayout.addWidget(&_hInsThickTitle);
    _insulationHThickLayout.addWidget(&_hInsThickValue);
    _insulationHThickLayout.addWidget(&_hInsThickUnit);

    _hInsResTitle.setMinimumWidth(200);
    _hInsResValue.setFixedWidth(50);
    _insulationHResLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _insulationHResLayout.addWidget(&_hInsResTitle);
    _insulationHResLayout.addWidget(&_hInsResValue);
    _insulationHResLayout.addWidget(&_hInsResUnit);

    _hInsLenTitle.setMinimumWidth(200);
    _hInsLenValue.setFixedWidth(50);
    _insulationHLenLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _insulationHLenLayout.addWidget(&_hInsLenTitle);
    _insulationHLenLayout.addWidget(&_hInsLenValue);
    _insulationHLenLayout.addWidget(&_hInsLenUnit);

    _insulationVLayout.addWidget(&_enableVInsulation);
    _insulationVLayout.addLayout(&_insulationVThickLayout);
    _insulationVLayout.addLayout(&_insulationVResLayout);
    _insulationVLayout.addLayout(&_insulationVLenLayout);

    _vInsThickTitle.setMinimumWidth(200);
    _vInsThickValue.setFixedWidth(50);
    _insulationVThickLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _insulationVThickLayout.addWidget(&_vInsThickTitle);
    _insulationVThickLayout.addWidget(&_vInsThickValue);
    _insulationVThickLayout.addWidget(&_vInsThickUnit);

    _vInsResTitle.setMinimumWidth(200);
    _vInsResValue.setFixedWidth(50);
    _insulationVResLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _insulationVResLayout.addWidget(&_vInsResTitle);
    _insulationVResLayout.addWidget(&_vInsResValue);
    _insulationVResLayout.addWidget(&_vInsResUnit);

    _vInsLenTitle.setMinimumWidth(200);
    _vInsLenValue.setFixedWidth(50);
    _insulationVLenLayout.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _insulationVLenLayout.addWidget(&_vInsLenTitle);
    _insulationVLenLayout.addWidget(&_vInsLenValue);
    _insulationVLenLayout.addWidget(&_vInsLenUnit);

    _floorDimensions.addLayout(&_insulationMeanFloor);
    _layout.addLayout(&_centralLayout);

    _resultsTitle.setFixedWidth(150);
    _resultsValue.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _resultsUnit.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _resultsUnit.setText("W/m");

    f.setBold(true);
    _resultsTitle.setFont(f);

    f.setUnderline(false);
    _resultsValue.setFont(f);
    _resultsUnit.setFont(f);

    _resultsLayout.addWidget(&_resultsTitle);
    _resultsLayout.addWidget(&_resultsValue);
    _resultsLayout.addWidget(&_resultsUnit);
    _resultsLayout.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _layout.addLayout(&_resultsLayout);

    _layout.addItem(new QSpacerItem(600, 6000, QSizePolicy::Expanding, QSizePolicy::Expanding));

    f=font();
    f.setUnderline(true);

    // Crawl.
    _crawlLayout.addLayout(&_crawlLayoutItem1);
    _crawlLayout.addLayout(&_crawlLayoutItem10);
    _crawlLayout.addLayout(&_crawlLayoutItem2);
    _crawlLayout.addLayout(&_crawlLayoutItem3);
    _crawlLayout.addLayout(&_crawlLayoutItem4);
    _crawlLayout.addLayout(&_crawlLayoutItem5);
    _crawlLayout.addLayout(&_crawlLayoutItem6);
    _crawlLayout.addLayout(&_crawlLayoutItem7);
    _crawlLayout.addLayout(&_crawlLayoutItem8);
    _crawlLayout.addLayout(&_crawlLayoutItem9);
    _crawlLayout.addItem(new QSpacerItem(0, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle1.setMinimumWidth(370);
    _crawlValue1.setFixedWidth(50);
    _crawlUnit1.setText("m");
    _crawlLayoutItem1.addWidget(&_crawlTitle1);
    _crawlLayoutItem1.addWidget(&_crawlValue1);
    _crawlLayoutItem1.addWidget(&_crawlUnit1);
    _crawlLayoutItem1.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle10.setMinimumWidth(370);
    _crawlValue10.setFixedWidth(50);
    _crawlUnit10.setText("m².°C/W");
    _crawlLayoutItem10.addWidget(&_crawlTitle10);
    _crawlLayoutItem10.addWidget(&_crawlValue10);
    _crawlLayoutItem10.addWidget(&_crawlUnit10);
    _crawlLayoutItem10.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle2.setMinimumWidth(370);
    _crawlValue2.setFixedWidth(50);
    _crawlUnit2.setText("m².°C/W");
    _crawlLayoutItem2.addWidget(&_crawlTitle2);
    _crawlLayoutItem2.addWidget(&_crawlValue2);
    _crawlLayoutItem2.addWidget(&_crawlUnit2);
    _crawlLayoutItem2.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle3.setMinimumWidth(370);
    _crawlValue3.setFixedWidth(50);
    _crawlUnit3.setText("m²/m");
    _crawlLayoutItem3.addWidget(&_crawlTitle3);
    _crawlLayoutItem3.addWidget(&_crawlValue3);
    _crawlLayoutItem3.addWidget(&_crawlUnit3);
    _crawlLayoutItem3.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle4.setMinimumWidth(370);
    _crawlValue4.setFixedWidth(50);
    _crawlUnit4.setText("m/s");
    _crawlLayoutItem4.addWidget(&_crawlTitle4);
    _crawlLayoutItem4.addWidget(&_crawlValue4);
    _crawlLayoutItem4.addWidget(&_crawlUnit4);
    _crawlLayoutItem4.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle5.setMinimumWidth(370);
    _crawlTitle5.setAlignment(Qt::AlignTop);
    _crawlLayoutItem5.addWidget(&_crawlTitle5);
    _crawlLayoutItem5.addLayout(&_windLayout);
    _windLayout.addLayout(&_wind1Layout);
    _windLayout.addLayout(&_wind2Layout);
    
    _group3.addButton(&_defaultWindTitle);
    _group3.addButton(&_userWindTitle);
    _defaultWindTitle.setChecked(true);

    _defaultWindValue.setFixedWidth(180);
    _wind1Layout.addWidget(&_defaultWindTitle);
    _wind1Layout.addWidget(&_defaultWindValue);
    _wind1Layout.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _userWindValue.setFixedWidth(50);
    _wind2Layout.addWidget(&_userWindTitle);
    _wind2Layout.addWidget(&_userWindValue);
    _wind2Layout.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle6.setMinimumWidth(370);
    _crawlValue6.setFixedWidth(50);
    _crawlUnit6.setText("m");
    _crawlLayoutItem6.addWidget(&_crawlTitle6);
    _crawlLayoutItem6.addWidget(&_crawlValue6);
    _crawlLayoutItem6.addWidget(&_crawlUnit6);
    _crawlLayoutItem6.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle7.setMinimumWidth(370);
    _crawlValue7.setFixedWidth(50);
    _crawlUnit7.setText("m².°C/W");
    _crawlLayoutItem7.addWidget(&_crawlTitle7);
    _crawlLayoutItem7.addWidget(&_crawlValue7);
    _crawlLayoutItem7.addWidget(&_crawlUnit7);
    _crawlLayoutItem7.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle8.setMinimumWidth(370);
    _crawlValue8.setFixedWidth(50);
    _crawlUnit8.setText("m².°C/W");
    _crawlLayoutItem8.addWidget(&_crawlTitle8);
    _crawlLayoutItem8.addWidget(&_crawlValue8);
    _crawlLayoutItem8.addWidget(&_crawlUnit8);
    _crawlLayoutItem8.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _crawlTitle9.setMinimumWidth(370);
    _crawlValue9.setFixedWidth(50);
    _crawlUnit9.setText("m².°C/W");
    _crawlLayoutItem9.addWidget(&_crawlTitle9);
    _crawlLayoutItem9.addWidget(&_crawlValue9);
    _crawlLayoutItem9.addWidget(&_crawlUnit9);
    _crawlLayoutItem9.addItem(new QSpacerItem(600, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    // Basement.
    _basementLayoutCommon1.addWidget(&_wallBasementLabel);
    _basementLayoutCommon1.addLayout(&_basementLayoutItem2);
    _basementLayoutCommon1.addLayout(&_basementLayoutItem3);
    _basementLayoutCommon1.addLayout(&_basementLayoutItem4);
    _basementLayoutCommon1.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _basementLayoutCommon2.addLayout(&_basementLayoutItem1);
    _basementLayoutCommon2.addWidget(&_basementHeatingType);
    _basementLayoutCommon2.addLayout(&_basementHeatingLayout1);
    _basementLayoutCommon2.addLayout(&_basementHeatingLayout2);
    _basementLayoutCommon2.addLayout(&_basementHeatingLayout3);
    _basementLayoutCommon2.addLayout(&_basementLayoutItem5);
    _basementLayoutCommon2.addLayout(&_basementLayoutItem6);
    _basementLayoutCommon2.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _group4.addButton(&_basementHeating1);
    _group4.addButton(&_basementHeating2);
    _group4.addButton(&_basementHeating3);
    _basementHeating1.setChecked(true);

    _basementHeatingLayout1.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementHeatingLayout1.addWidget(&_basementHeating1);

    _basementHeatingLayout2.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementHeatingLayout2.addWidget(&_basementHeating2);

    _basementHeatingLayout3.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementHeatingLayout3.addWidget(&_basementHeating3);
    
    _basementLayoutCommon.addLayout(&_basementLayoutCommon1);
    _basementLayoutCommon.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutCommon.addLayout(&_basementLayoutCommon2);
    _basementLayoutCommon.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _basementLayout.addLayout(&_basementLayoutCommon);

    _basementLayout.addLayout(&_basementLayoutItem7);
    _basementLayout.addLayout(&_basementLayoutItem10);
    _basementLayout.addLayout(&_basementLayoutItem11);

    _basementExtWallTitle.setFont(f);
    _basementLayout.addWidget(&_basementExtWallTitle);
    _basementLayout.addLayout(&_basementLayoutItem8);
    _basementLayout.addLayout(&_basementLayoutItem9);

    _basementTitle1.setMinimumWidth(150);
    _basementValue1.setFixedWidth(50);
    _basementUnit1.setText("m");
    _basementLayoutItem1.addWidget(&_basementTitle1);
    _basementLayoutItem1.addWidget(&_basementValue1);
    _basementLayoutItem1.addWidget(&_basementUnit1);

    _wallBasementLabel.setFont(f);

    _basementTitle2.setMinimumWidth(200);
    _basementValue2.setFixedWidth(50);
    _basementUnit2.setText("m².°C/W");
    _basementLayoutItem2.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem2.addWidget(&_basementTitle2);
    _basementLayoutItem2.addWidget(&_basementValue2);
    _basementLayoutItem2.addWidget(&_basementUnit2);

    _basementTitle3.setMinimumWidth(200);
    _basementValue3.setFixedWidth(50);
    _basementUnit3.setText("m².°C/W");
    _basementLayoutItem3.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem3.addWidget(&_basementTitle3);
    _basementLayoutItem3.addWidget(&_basementValue3);
    _basementLayoutItem3.addWidget(&_basementUnit3);

    _basementTitle4.setMinimumWidth(200);
    _basementValue4.setFixedWidth(50);
    _basementUnit4.setText("m².°C/W");
    _basementLayoutItem4.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem4.addWidget(&_basementTitle4);
    _basementLayoutItem4.addWidget(&_basementValue4);
    _basementLayoutItem4.addWidget(&_basementUnit4);

    _basementTitle5.setMinimumWidth(110);
    _basementValue5.setFixedWidth(50);
    _basementUnit5.setText("m²");
    _basementLayoutItem5.addItem(new QSpacerItem(60, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem5.addWidget(&_basementTitle5);
    _basementLayoutItem5.addWidget(&_basementValue5);
    _basementLayoutItem5.addWidget(&_basementUnit5);

    _basementTitle6.setMinimumWidth(110);
    _basementValue6.setFixedWidth(50);
    _basementUnit6.setText("m²");
    _basementLayoutItem6.addItem(new QSpacerItem(60, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem6.addWidget(&_basementTitle6);
    _basementLayoutItem6.addWidget(&_basementValue6);
    _basementLayoutItem6.addWidget(&_basementUnit6);

    _basementTitle7.setMinimumWidth(230);
    _basementValue7.setFixedWidth(50);
    _basementUnit7.setText("m².°C/W");
    _basementLayoutItem7.addWidget(&_basementTitle7);
    _basementLayoutItem7.addWidget(&_basementValue7);
    _basementLayoutItem7.addWidget(&_basementUnit7);
    _basementLayoutItem7.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _basementTitle8.setMinimumWidth(210);
    _basementValue8.setFixedWidth(50);
    _basementUnit8.setText("m");
    _basementLayoutItem8.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem8.addWidget(&_basementTitle8);
    _basementLayoutItem8.addWidget(&_basementValue8);
    _basementLayoutItem8.addWidget(&_basementUnit8);
    _basementLayoutItem8.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _basementTitle9.setMinimumWidth(210);
    _basementValue9.setFixedWidth(50);
    _basementUnit9.setText("m².°C/W");
    _basementLayoutItem9.addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _basementLayoutItem9.addWidget(&_basementTitle9);
    _basementLayoutItem9.addWidget(&_basementValue9);
    _basementLayoutItem9.addWidget(&_basementUnit9);
    _basementLayoutItem9.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _basementTitle10.setMinimumWidth(230);
    _basementValue10.setFixedWidth(50);
    _basementUnit10.setText("m");
    _basementLayoutItem10.addWidget(&_basementTitle10);
    _basementLayoutItem10.addWidget(&_basementValue10);
    _basementLayoutItem10.addWidget(&_basementUnit10);
    _basementLayoutItem10.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _basementTitle11.setMinimumWidth(230);
    _basementValue11.setFixedWidth(50);
    _basementUnit11.setText("vol./h");
    _basementLayoutItem11.addWidget(&_basementTitle11);
    _basementLayoutItem11.addWidget(&_basementValue11);
    _basementLayoutItem11.addWidget(&_basementUnit11);
    _basementLayoutItem11.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _grid.addLayout(&_layout, 0, 0, 1, 1);
    _grid.addWidget(&_helpImage, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignBottom);
    _grid.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 1, 1, 1);
    _grid.addItem(new QSpacerItem(600, 600, QSizePolicy::Expanding, QSizePolicy::Expanding), 1, 0, 1, 1);
    _grid.setColumnStretch(0, 1000);
    _grid.setRowStretch(0, 1000);
    _grid.setRowMinimumHeight(0, 650);

    _crawlWidget.hide();
    _basementWidget.hide();

    _centralLayout.addWidget(&_crawlWidget);
    _centralLayout.addWidget(&_basementWidget);
    _basementLayout.setContentsMargins(0,0,0,0);
    _crawlLayout.setContentsMargins(0,0,0,0);

    translate();

    connect(&_typeValue, &QComboBox::currentIndexChanged, this, &En13370Interface::typeChanged);

    connect(&_floorWidth, &QAbstractButton::clicked, this, &En13370Interface::updateVisibility);
    connect(&_floorAreaAndPerimeter,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_groundMaterial, &QAbstractButton::clicked, this, &En13370Interface::updateVisibility);
    connect(&_groundConductivity,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_enableHInsulation,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_enableVInsulation,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_defaultWindTitle,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_userWindTitle, &QAbstractButton::clicked, this, &En13370Interface::updateVisibility);
    connect(&_basementHeating1,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_basementHeating2,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);
    connect(&_basementHeating3,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateVisibility);

    connect(&_crawlValue6, &QLineEdit::textEdited, this, &En13370Interface::updateVisibility);

    connect(qApp,
            &QApplication::focusChanged,
            this,
            &En13370Interface::focusChanged);

    connect(&_floorWidthValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_floorAreaValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_floorPerimeterValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_modelFloorWidthValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_groundConductivityValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_intTempValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_extTempValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_wallThicknessValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_groundRsiValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_groundRValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_groundRseValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_hInsThickValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_vInsThickValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_hInsResValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_vInsResValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_hInsLenValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_vInsLenValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue1, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue10, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue2, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue3, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue4, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue6, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue7, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue8, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_crawlValue9, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_userWindValue, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue1, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue2, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue3, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue4, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue5, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue6, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue7, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue8, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue9, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue10, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);
    connect(&_basementValue11, &QLineEdit::textEdited, this, &En13370Interface::dataChanged);

    connect(&_floorWidth, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_floorAreaAndPerimeter,
            &QAbstractButton::clicked,
            this,
            &En13370Interface::updateResults);
    connect(&_groundMaterial, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_groundConductivity, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_defaultWindTitle, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_userWindTitle, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_basementHeating1, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_basementHeating2, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_basementHeating3, &QAbstractButton::clicked, this, &En13370Interface::updateResults);

    connect(&_groundMaterialValue,
            &QComboBox::currentTextChanged,
            this,
            &En13370Interface::dataChanged);
    connect(&_intEnvValue, &QComboBox::currentTextChanged, this, &En13370Interface::dataChanged);
    connect(&_extEnvValue, &QComboBox::currentTextChanged, this, &En13370Interface::dataChanged);
    connect(&_typeValue, &QComboBox::currentTextChanged, this, &En13370Interface::dataChanged);
    connect(&_defaultWindValue,
            &QComboBox::currentTextChanged,
            this,
            &En13370Interface::dataChanged);

    connect(&_enableHInsulation, &QAbstractButton::clicked, this, &En13370Interface::updateResults);
    connect(&_enableVInsulation, &QAbstractButton::clicked, this, &En13370Interface::updateResults);

    updateVisibility();
    updateResults();
    applyTheme();
}

En13370Interface::~En13370Interface()
{
}

void En13370Interface::loadPlugin(HeatFlowComputerPlugin *plugin)
{
    _plugin=dynamic_cast<En13370Plugin*>(plugin);

    if (!_plugin)
        return;

    _importData=true;

    // Floor width.
    bool useWidth=_plugin->useFloorWidth();
    double width=_plugin->floorWidth();
    double perimeter=_plugin->perimeter();
    double area=_plugin->area();

    if (useWidth)
    {
        _floorWidth.setChecked(true);
        _floorWidthValue.setText(toString(width));
    }
    else
    {
        _floorAreaAndPerimeter.setChecked(true);
        _floorAreaValue.setText(toString(area));
        _floorPerimeterValue.setText(toString(perimeter));
    }

    double model_width=_plugin->modelFloorWidth();
    _modelFloorWidthValue.setText(toString(model_width));

    // Ground conductivity.
    bool useConductivity=_plugin->useConductivity();
    double conductivity=_plugin->groundConductivity();
    std::string materialId=_plugin->materialId();

    if (useConductivity)
    {
        _groundConductivity.setChecked(true);
        _groundConductivityValue.setText(toString(conductivity));
    }
    else
    {
        _groundMaterial.setChecked(true);
        _groundMaterialValue.setCurrentIndex(-1);
        for (int i=0 ; i<_groundMaterialValue.count() ; i++)
        {
            if (_groundMaterialValue.itemData(i).toString().toStdString()==materialId)
            {
                _groundMaterialValue.setCurrentIndex(i);
                break;
            }
        }
    }

    // Get temperatures.
    _intTempValue.setText(toString(_plugin->internalTemperature()));
    _extTempValue.setText(toString(_plugin->externalTemperature()));

    int int_idx=QString(_plugin->internalEnvironment().c_str()).toInt();
    int ext_idx=QString(_plugin->externalEnvironment().c_str()).toInt();

    _intEnvValue.setCurrentIndex(int_idx);
    _extEnvValue.setCurrentIndex(ext_idx);

    // Common part.
    _wallThicknessValue.setText(toString(_plugin->wallThickness()*1000.0));
    _groundRsiValue.setText(toString(_plugin->floorInternalSuperficialResistance()));
    _groundRValue.setText(toString(_plugin->floorResistance()));
    _groundRseValue.setText(toString(_plugin->floorExternalSuperficialResistance()));

    // Type.
    if (_plugin->type()==En13370Plugin::Median)
        _typeValue.setCurrentIndex(0);
    else if (_plugin->type()==En13370Plugin::CrawlSpace)
        _typeValue.setCurrentIndex(1);
    else
        _typeValue.setCurrentIndex(2);

    // Horizontal insulation.
    if (_plugin->includeHorizontalInsulation())
    {
        _enableHInsulation.setChecked(true);
        _hInsThickValue.setText(toString(_plugin->horizontalInsulationThickness()*1000.0));
        _hInsResValue.setText(toString(_plugin->horizontalInsulationResistance()));
        _hInsLenValue.setText(toString(_plugin->insulationWidth()));
    }
    else
    {
        _enableHInsulation.setChecked(false);
        _hInsThickValue.setText("");
        _hInsResValue.setText("");
        _hInsLenValue.setText("");
    }

    // Vertical insulation.
    if (_plugin->includeVerticalInsulation())
    {
        _enableVInsulation.setChecked(true);
        _vInsThickValue.setText(toString(_plugin->verticalInsulationThickness()*1000.0));
        _vInsResValue.setText(toString(_plugin->verticalInsulationResistance()));
        _vInsLenValue.setText(toString(_plugin->insulationHeight()));
    }
    else
    {
        _enableVInsulation.setChecked(false);
        _vInsThickValue.setText("");
        _vInsResValue.setText("");
        _vInsLenValue.setText("");
    }

    // Crawl.
    if (_plugin->type()==En13370Plugin::CrawlSpace)
    {
        double ceil_r=_plugin->zoneCeilResistance();
        double height=_plugin->crawlHeight();
        double ext_wall_R=_plugin->externalWallResistance();
        double area_ratio=_plugin->openingsRatio();
        double wind_speed=_plugin->windSpeed();
        double wind_factor=_plugin->windProtectionFactor();
        double depth=_plugin->undergroundDepth();

        double ground_wall_rsi=_plugin->wallInternalSuperficialResistance();
        double ground_wall_r=_plugin->wallResistance();
        double ground_wall_rse=_plugin->wallExternalSuperficialResistance();

        _crawlValue10.setText(toString(ceil_r));
        _crawlValue1.setText(toString(height));
        _crawlValue2.setText(toString(ext_wall_R));
        _crawlValue3.setText(toString(area_ratio));
        _crawlValue4.setText(toString(wind_speed));
        _crawlValue6.setText(toString(depth));

        _crawlValue7.setText(toString(ground_wall_rsi));
        _crawlValue8.setText(toString(ground_wall_r));
        _crawlValue9.setText(toString(ground_wall_rse));

        if (wind_factor==0.02 || wind_factor==0.05 || wind_factor==0.10)
        {
            _defaultWindTitle.setChecked(true);
            if (wind_factor==0.02)
                _defaultWindValue.setCurrentIndex(0);
            else if (wind_factor==0.05)
                _defaultWindValue.setCurrentIndex(1);
            else if (wind_factor==0.10)
                _defaultWindValue.setCurrentIndex(2);
        }
        else
        {
            _userWindTitle.setChecked(true);
            _userWindValue.setText(toString(wind_factor));
        }
    }

    // Basement.
    if (_plugin->type()==En13370Plugin::HeatedUnderground || _plugin->type()==En13370Plugin::NotHeatedUnderground || _plugin->type()==En13370Plugin::PartiallyHeatedUnderground)
    {
        double depth=_plugin->undergroundDepth();
        double wall_rsi=_plugin->wallInternalSuperficialResistance();
        double wall_r=_plugin->wallResistance();
        double wall_rse=_plugin->wallExternalSuperficialResistance();

        double ceil_r=_plugin->zoneCeilResistance();
        double mean_height=_plugin->meanZoneHeight();
        double air_rate=_plugin->airExchangeRate();
        double ext_wall_height=_plugin->externalWellHeight();
        double ext_wall_r=_plugin->externalWallResistance();

        double heated_area=_plugin->heatedUndergroundArea();
        double unheated_area=_plugin->notHeatedUndergroundArea();

        _basementValue1.setText(toString(depth));
        _basementValue2.setText(toString(wall_rsi));
        _basementValue3.setText(toString(wall_r));
        _basementValue4.setText(toString(wall_rse));

        _basementValue7.setText(toString(ceil_r));
        _basementValue10.setText(toString(mean_height));
        _basementValue11.setText(toString(air_rate));
        _basementValue8.setText(toString(ext_wall_height));
        _basementValue9.setText(toString(ext_wall_r));

        if(_plugin->type()==En13370Plugin::NotHeatedUnderground)
            _basementHeating1.setChecked(true);
        else if(_plugin->type()==En13370Plugin::HeatedUnderground)
            _basementHeating2.setChecked(true);
        else if(_plugin->type()==En13370Plugin::PartiallyHeatedUnderground)
            _basementHeating3.setChecked(true);

        if (_plugin->type()==En13370Plugin::PartiallyHeatedUnderground)
        {
            _basementValue5.setText(toString(heated_area));
            _basementValue6.setText(toString(unheated_area));
        }
        else
        {
            _basementValue5.setText("");
            _basementValue6.setText("");
        }
    }

    _importData=false;
    updateVisibility();
    updateResults();
}

QString En13370Interface::toString(double value) const
{
    QLocale locale;
    return locale.toString(value);
}

void En13370Interface::translate()
{
    _floorDimensionsLabel.setText(_tr("Dimensions du plancher"));
    _floorWidth.setText(_tr("Largeur :"));
    _floorAreaAndPerimeter.setText(_tr("Aire :"));
    _floorPerimeter.setText(_tr("Périmètre :"));
    _modelFloorWidth.setText(_tr("Largeur de plancher modélisée :"));
    _groundProperties.setText(_tr("Propriétés thermiques du sol"));
    _groundMaterial.setText(_tr("choix d'un matériau :"));
    _groundConductivity.setText(_tr("conductivité thermique :"));
    _conditionsTitle.setText(_tr("Ambiances thermiques"));
    _intTempTitle.setText(_tr("Température intérieure :"));
    _extTempTitle.setText(_tr("Température extérieure :"));
    _intEnvTitle.setText(_tr("Ambiance thermique intérieure :"));
    _extEnvTitle.setText(_tr("Ambiance thermique extérieure :"));
    _typeTitle.setText(_tr("Type de plancher bas :"));

    _typeValue.clear();
    _typeValue.addItem(_tr("Plancher bas sur terre-plein"), En13370Plugin::Median);
    _typeValue.addItem(_tr("Vide sanitaire"), En13370Plugin::CrawlSpace);
    _typeValue.addItem(_tr("Sous-sol"), En13370Plugin::HeatedUnderground);

    _resultsTitle.setText(_tr("Flux de chaleur sortant :"));
    _wallThicknessTitle.setText(_tr("Epaisseur totale du mur :"));
    _groundFloorTitle.setText(_tr("Plancher au contact du sol"));
    _groundRsiTitle.setText(_tr("Résistance superficielle intérieure :"));
    _groundRTitle.setText(_tr("Résistance thermique du plancher :"));
    _groundRseTitle.setText(_tr("Résistance superficielle extérieure :"));

    _insulationTitle.setText(_tr("Isolation périphérique"));
    _hInsThickTitle.setText(_tr("Epaisseur de l'isolation :"));
    _vInsThickTitle.setText(_tr("Epaisseur de l'isolation :"));
    _hInsResTitle.setText(_tr("Résistance thermique de l'isolation :"));
    _vInsResTitle.setText(_tr("Résistance thermique de l'isolation :"));
    _hInsLenTitle.setText(_tr("Longueur de l'isolation :"));
    _vInsLenTitle.setText(_tr("Hauteur de l'isolation :"));
    _enableHInsulation.setText(_tr("inclure une isolation périphérique horizontale"));
    _enableVInsulation.setText(_tr("inclure une isolation périphérique verticale"));

    _crawlTitle1.setText(_tr("Hauteur du vide sanitaire par rapport au sol :"));
    _crawlTitle10.setText(_tr("Résistance thermique du plafond du vide sanitaire :"));
    _crawlTitle2.setText(_tr("Résistance thermique du mur donnant sur l'extérieur :"));
    _crawlTitle3.setText(_tr("Aire des ouvertures de ventilation divisée par le périmètre du vide sanitaire :"));
    _crawlTitle4.setText(_tr("Vitesse moyenne du vent à 10 m de hauteur :"));
    _crawlTitle5.setText(_tr("Facteur de protection au vent :"));
    _crawlTitle6.setText(_tr("Profondeur moyenne du vide sanitaire :"));
    _crawlTitle7.setText(_tr("Résistance superficielle intérieure du mur donnant sur le sol :"));
    _crawlTitle8.setText(_tr("Résistance thermique du mur donnant sur le sol :"));
    _crawlTitle9.setText(_tr("Résistance superficielle extérieure du mur donnant sur le sol :"));

    _defaultWindTitle.setText(_tr("valeur par défaut :"));
    _userWindTitle.setText(_tr("valeur fixe :"));

    _defaultWindValue.clear();
    _defaultWindValue.addItem(_tr("Abritée - Centre ville (0,02)"), 0.02);
    _defaultWindValue.addItem(_tr("Moyenne - Banlieue (0,05)"), 0.05);
    _defaultWindValue.addItem(_tr("Exposée - Milieu rural (0,10)"), 0.1);

    _basementTitle1.setText(_tr("Profondeur du sous-sol :"));
    _basementTitle2.setText(_tr("Résistance superficielle intérieure :"));
    _basementTitle3.setText(_tr("Résistance superficielle extérieure :"));
    _basementTitle4.setText(_tr("Résistance thermique du mur :"));
    _basementTitle5.setText(_tr("Surface chauffée :"));
    _basementTitle6.setText(_tr("Surface non chauffée :"));
    _basementTitle7.setText(_tr("Résistance thermique du plafond du sous-sol :"));
    _basementTitle8.setText(_tr("Hauteur :"));
    _basementTitle9.setText(_tr("Résistance thermique :"));
    _basementTitle10.setText(_tr("Hauteur moyenne du sous-sol :"));
    _basementTitle11.setText(_tr("Taux de renouvellement d'air :"));

    _wallBasementLabel.setText(_tr("Mur du sous-sol"));
    _basementHeatingType.setText(_tr("Chauffage du sous-sol :"));

    _basementHeating1.setText(_tr("sous-sol non chauffé"));
    _basementHeating2.setText(_tr("sous-sol chauffé"));
    _basementHeating3.setText(_tr("sous-sol partiellement chauffé"));
    _basementExtWallTitle.setText(_tr("Mur du sous-sol donnant sur l'extérieur"));

    _intEnvValue.clear();
    _intEnvValue.addItem(_tr("Ambiance non définie"));
    _intEnvValue.addItem(_tr("Ambiance thermique 1"));
    _intEnvValue.addItem(_tr("Ambiance thermique 2"));
    _intEnvValue.addItem(_tr("Ambiance thermique 3"));
    _intEnvValue.addItem(_tr("Ambiance thermique 4"));

    _extEnvValue.clear();
    _extEnvValue.addItem(_tr("Ambiance non définie"));
    _extEnvValue.addItem(_tr("Ambiance thermique 1"));
    _extEnvValue.addItem(_tr("Ambiance thermique 2"));
    _extEnvValue.addItem(_tr("Ambiance thermique 3"));
    _extEnvValue.addItem(_tr("Ambiance thermique 4"));
}

void En13370Interface::applyTheme()
{
    _13370_basement_ceil_resistance = QPixmap(":/images/orange-blue/13370_basement_ceil_resistance.png");
    _13370_basement_external_wall = QPixmap(":/images/orange-blue/13370_basement_external_wall.png");
    _13370_basement_external_wall_height = QPixmap(":/images/orange-blue/13370_basement_external_wall_height.png");
    _13370_basement_height = QPixmap(":/images/orange-blue/13370_basement_height.png");
    _13370_basement_mean_height = QPixmap(":/orange-blue/green/13370_basement_mean_height.png");
    _13370_basement_wall = QPixmap(":/images/orange-blue/13370_basement_wall.png");
    _13370_crawl_depth = QPixmap(":/images/orange-blue/13370_crawl_depth.png");
    _13370_crawl_height = QPixmap(":/images/orange-blue/13370_crawl_height.png");
    _13370_crawl_wall = QPixmap(":/images/orange-blue/13370_crawl_wall.png");
    _13370_external = QPixmap(":/images/orange-blue/13370_external.png");
    _13370_floor_area = QPixmap(":/images/orange-blue/13370_floor_area.png");
    _13370_floor_perimeter = QPixmap(":/images/orange-blue/13370_floor_perimeter.png");
    _13370_floor_resistance_1 = QPixmap(":/images/orange-blue/13370_floor_resistance_1.png");
    _13370_floor_resistance_2 = QPixmap(":/images/orange-blue/13370_floor_resistance_2.png");
    _13370_floor_resistance_3 = QPixmap(":/images/orange-blue/13370_floor_resistance_3.png");
    _13370_floor_width = QPixmap(":/images/orange-blue/13370_floor_width.png");
    _13370_ground_conductivity = QPixmap(":/images/orange-blue/13370_ground_conductivity.png");
    _13370_internal = QPixmap(":/images/orange-blue/13370_internal.png");
    _13370_model_floor_width = QPixmap(":/images/orange-blue/13370_model_floor_width.png");
    _13370_wall_thickness_1 = QPixmap(":/images/orange-blue/13370_wall_thickness_1.png");
    _13370_wall_thickness_2 = QPixmap(":/images/orange-blue/13370_wall_thickness_2.png");
    _13370_wall_thickness_3 = QPixmap(":/images/orange-blue/13370_wall_thickness_3.png");
    _13370_crawl_wall_ground = QPixmap(":/images/orange-blue/13370_crawl_wall_ground.png");
    _13370_iso_h_resistance = QPixmap(":/images/orange-blue/13370_iso_h_resistance.png");
    _13370_iso_h_thickness = QPixmap(":/images/orange-blue/13370_iso_h_thickness.png");
    _13370_iso_h_width = QPixmap(":/images/orange-blue/13370_iso_h_width.png");
    _13370_iso_v_height = QPixmap(":/images/orange-blue/13370_iso_v_height.png");
    _13370_iso_v_resistance = QPixmap(":/images/orange-blue/13370_iso_v_resistance.png");
    _13370_iso_v_thickness = QPixmap(":/images/orange-blue/13370_iso_v_thickness.png");
    update();
}

void En13370Interface::clean()
{
    _floorWidthValue.setText("");
    _floorAreaValue.setText("");
    _floorPerimeterValue.setText("");
    _modelFloorWidthValue.setText("");
    _groundConductivityValue.setText("");
}

void En13370Interface::typeChanged(int type)
{
    _crawlWidget.hide();
    _basementWidget.hide();

    if (type==1)
        _crawlWidget.show();
    else if (type==2)
        _basementWidget.show();
}



void En13370Interface::updateVisibility()
{
    _floorWidthValue.setEnabled(_floorWidth.isChecked());
    _floorAreaValue.setEnabled(!_floorWidth.isChecked());
    _floorPerimeterValue.setEnabled(!_floorWidth.isChecked());

    _groundMaterialValue.setEnabled(_groundMaterial.isChecked());
    _groundConductivityValue.setEnabled(!_groundMaterial.isChecked());

    _hInsThickValue.setEnabled(_enableHInsulation.isChecked());
    _hInsResValue.setEnabled(_enableHInsulation.isChecked());
    _hInsLenValue.setEnabled(_enableHInsulation.isChecked());

    _vInsThickValue.setEnabled(_enableVInsulation.isChecked());
    _vInsResValue.setEnabled(_enableVInsulation.isChecked());
    _vInsLenValue.setEnabled(_enableVInsulation.isChecked());

    _defaultWindValue.setEnabled(_defaultWindTitle.isChecked());
    _userWindValue.setEnabled(!_defaultWindTitle.isChecked());

    _basementValue5.setEnabled(_basementHeating3.isChecked());
    _basementValue6.setEnabled(_basementHeating3.isChecked());

    _basementExtWallTitle.setVisible(!_basementHeating2.isChecked());

    _basementTitle7.setVisible(!_basementHeating2.isChecked());
    _basementTitle8.setVisible(!_basementHeating2.isChecked());
    _basementTitle9.setVisible(!_basementHeating2.isChecked());
    _basementTitle10.setVisible(!_basementHeating2.isChecked());
    _basementTitle11.setVisible(!_basementHeating2.isChecked());

    _basementUnit7.setVisible(!_basementHeating2.isChecked());
    _basementUnit8.setVisible(!_basementHeating2.isChecked());
    _basementUnit9.setVisible(!_basementHeating2.isChecked());
    _basementUnit10.setVisible(!_basementHeating2.isChecked());
    _basementUnit11.setVisible(!_basementHeating2.isChecked());

    _basementValue7.setVisible(!_basementHeating2.isChecked());
    _basementValue8.setVisible(!_basementHeating2.isChecked());
    _basementValue9.setVisible(!_basementHeating2.isChecked());
    _basementValue10.setVisible(!_basementHeating2.isChecked());
    _basementValue11.setVisible(!_basementHeating2.isChecked());

    _crawlValue7.setVisible(readDouble(_crawlValue6.text())>0.5);
    _crawlValue8.setVisible(readDouble(_crawlValue6.text())>0.5);
    _crawlValue9.setVisible(readDouble(_crawlValue6.text())>0.5);

    _crawlUnit7.setVisible(readDouble(_crawlValue6.text())>0.5);
    _crawlUnit8.setVisible(readDouble(_crawlValue6.text())>0.5);
    _crawlUnit9.setVisible(readDouble(_crawlValue6.text())>0.5);

    _crawlTitle7.setVisible(readDouble(_crawlValue6.text())>0.5);
    _crawlTitle8.setVisible(readDouble(_crawlValue6.text())>0.5);
    _crawlTitle9.setVisible(readDouble(_crawlValue6.text())>0.5);
}

double En13370Interface::readDouble(const QString &value) const
{
    QString s=value;
    s.replace(",", ".");
    return s.toDouble();
}

void En13370Interface::focusChanged(QWidget * old, QWidget * now)
{
    int image_height=150;

    if (now==&_floorWidthValue || now==&_floorWidth)
        _helpImage.setPixmap(_13370_floor_width.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_floorAreaValue  || now==&_floorAreaAndPerimeter)
        _helpImage.setPixmap(_13370_floor_area.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_floorPerimeterValue)
        _helpImage.setPixmap(_13370_floor_perimeter.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_modelFloorWidthValue)
        _helpImage.setPixmap(_13370_model_floor_width.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundMaterial || now==&_groundMaterialValue || now==&_groundConductivity || now==&_groundConductivityValue)
        _helpImage.setPixmap(_13370_ground_conductivity.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_intTempValue || now==&_intEnvValue)
        _helpImage.setPixmap(_13370_internal.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_extTempValue || now==&_extEnvValue)
        _helpImage.setPixmap(_13370_external.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_wallThicknessValue && _typeValue.currentIndex()==0)
        _helpImage.setPixmap(_13370_wall_thickness_1.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_wallThicknessValue && _typeValue.currentIndex()==1)
        _helpImage.setPixmap(_13370_wall_thickness_2.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_wallThicknessValue && _typeValue.currentIndex()==2)
        _helpImage.setPixmap(_13370_wall_thickness_3.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_groundRsiValue && _typeValue.currentIndex()==0)
        _helpImage.setPixmap(_13370_floor_resistance_1.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundRValue && _typeValue.currentIndex()==0)
        _helpImage.setPixmap(_13370_floor_resistance_1.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundRseValue && _typeValue.currentIndex()==0)
        _helpImage.setPixmap(_13370_floor_resistance_1.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_groundRsiValue && _typeValue.currentIndex()==1)
        _helpImage.setPixmap(_13370_floor_resistance_2.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundRValue && _typeValue.currentIndex()==1)
        _helpImage.setPixmap(_13370_floor_resistance_2.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundRseValue && _typeValue.currentIndex()==1)
        _helpImage.setPixmap(_13370_floor_resistance_2.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_groundRsiValue && _typeValue.currentIndex()==2)
        _helpImage.setPixmap(_13370_floor_resistance_3.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundRValue && _typeValue.currentIndex()==2)
        _helpImage.setPixmap(_13370_floor_resistance_3.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_groundRseValue && _typeValue.currentIndex()==2)
        _helpImage.setPixmap(_13370_floor_resistance_3.scaledToHeight(image_height, Qt::SmoothTransformation));
    
    else if (now==&_crawlValue1)
        _helpImage.setPixmap(_13370_crawl_height.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_crawlValue10)
        _helpImage.setPixmap(_13370_basement_ceil_resistance.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_crawlValue2)
        _helpImage.setPixmap(_13370_crawl_wall.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_crawlValue3)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_crawlValue4)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_defaultWindTitle || now==&_userWindTitle || now==&_defaultWindValue || now==&_userWindValue)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_crawlValue6)
        _helpImage.setPixmap(_13370_crawl_depth.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_crawlValue7 || now==&_crawlValue8 || now==&_crawlValue9)
        _helpImage.setPixmap(_13370_crawl_wall_ground.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_basementValue1)
        _helpImage.setPixmap(_13370_basement_height.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue2)
        _helpImage.setPixmap(_13370_basement_wall.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue3)
        _helpImage.setPixmap(_13370_basement_wall.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue4)
        _helpImage.setPixmap(_13370_basement_wall.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue5)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_basementValue6)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_basementValue7)
        _helpImage.setPixmap(_13370_basement_ceil_resistance.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue8)
        _helpImage.setPixmap(_13370_basement_external_wall_height.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue9)
        _helpImage.setPixmap(_13370_basement_external_wall.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue10)
        _helpImage.setPixmap(_13370_basement_mean_height.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_basementValue11)
        _helpImage.setPixmap(_emptyPixmap);

    else if (now==&_basementHeating1)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_basementHeating2)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_basementHeating3)
        _helpImage.setPixmap(_emptyPixmap);

    else if (now==&_enableHInsulation)
        _helpImage.setPixmap(_emptyPixmap);
    else if (now==&_enableVInsulation)
        _helpImage.setPixmap(_emptyPixmap);

    else if (now==&_hInsThickValue)
        _helpImage.setPixmap(_13370_iso_h_thickness.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_hInsResValue)
        _helpImage.setPixmap(_13370_iso_h_resistance.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_hInsLenValue)
        _helpImage.setPixmap(_13370_iso_h_width.scaledToHeight(image_height, Qt::SmoothTransformation));

    else if (now==&_vInsThickValue)
        _helpImage.setPixmap(_13370_iso_v_thickness.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_vInsResValue)
        _helpImage.setPixmap(_13370_iso_v_resistance.scaledToHeight(image_height, Qt::SmoothTransformation));
    else if (now==&_vInsLenValue)
        _helpImage.setPixmap(_13370_iso_v_height.scaledToHeight(image_height, Qt::SmoothTransformation));
}

double En13370Interface::floorWidth() const
{
    if (_floorWidth.isChecked())
    {
        double w=std::max(readDouble(_floorWidthValue.text()), 0.0);
        if (_plugin)
        {
            _plugin->useFloorWidth(true);
            _plugin->setFloorWidth(w);
        }
        return w;
    }

    // Compute width.
    double A=readDouble(_floorAreaValue.text());
    double P=readDouble(_floorPerimeterValue.text());

    if (_plugin)
    {
        _plugin->useFloorWidth(false);
        _plugin->setPerimeter(P);
        _plugin->setArea(A);
    }
    
    if (P<=0.0)
        return 0.0;
    if (A<=0.0)
        return 0.0;

    return A/(0.5*P);
}

double En13370Interface::groundConductivity() const
{
    if (_groundConductivity.isChecked())
    {
        double l=std::max(readDouble(_groundConductivityValue.text()), 0.0);
        if (_plugin)
        {
            _plugin->useConductivity(true);
            _plugin->setGroundConductivity(l);
        }
        return l;
    }

    std::string id=_groundMaterialValue.currentData().toString().toStdString();

    if (_plugin)
    {
        _plugin->useConductivity(false);
        _plugin->setMaterialId(id);
    }

    Material *material=Materials::instance()->materialFromId(id);
    if (!material)
        return 0.0;

    if (_plugin)
        _plugin->setGroundConductivity(material->conductivity());

    return material->conductivity();
}

void En13370Interface::dataChanged(const QString &data)
{
    updateResults();
}

void En13370Interface::invalidateResults()
{
    _resultsValue.setText(" - ");
}

void En13370Interface::displayResults(double flux)
{
    QLocale locale;
    QString value=locale.toString(flux, 'f', 3);
    _resultsValue.setText(value);
}

En13370Plugin::LowFloorType En13370Interface::floorType() const
{
    if (_typeValue.currentIndex()==0)
        return En13370Plugin::Median;
    if (_typeValue.currentIndex()==1)
        return En13370Plugin::CrawlSpace;

    // Basement.
    if (_basementHeating1.isChecked())
        return En13370Plugin::NotHeatedUnderground;
    if (_basementHeating2.isChecked())
        return En13370Plugin::HeatedUnderground;
    return En13370Plugin::PartiallyHeatedUnderground;
}

double En13370Interface::windFactor() const
{
    if (_userWindTitle.isChecked())
        return std::max(0.0, readDouble(_userWindValue.text()));

    if (_defaultWindValue.currentIndex()==0)
        return 0.02;
    if (_defaultWindValue.currentIndex()==1)
        return 0.05;
    return 0.10;
}

void En13370Interface::updateResults()
{
    if (_importData)
        return;

    // Get plugin.
    if (!_plugin)
        return invalidateResults();

    // HACK: save data.
    groundConductivity();
    floorWidth();

    // Get floor width.
    double floor_width=floorWidth();
    if (floor_width<=0.0)
        return invalidateResults();
    _plugin->setFloorWidth(floor_width);

    // Get ground conductivity.
    double conductivity=groundConductivity();
    if (conductivity<=0.0)
        return invalidateResults();
    _plugin->setGroundConductivity(conductivity);

    // Get model floor width.
    double model_floor_width=std::max(readDouble(_modelFloorWidthValue.text()), 0.0);
    _plugin->setModelFloorWidth(model_floor_width);

    // Get type.
    En13370Plugin::LowFloorType type=floorType();
    _plugin->setType(type);

    // Get temperatures & environments.
    double internal_temperature=readDouble(_intTempValue.text());
    double external_temperature=readDouble(_extTempValue.text());
    _plugin->setInternalTemperature(internal_temperature);
    _plugin->setExternalTemperature(external_temperature);

    QString internal_env=QString::number(_intEnvValue.currentIndex());
    QString external_env=QString::number(_extEnvValue.currentIndex());
    _plugin->setInternalEnvironment(internal_env.toStdString());
    _plugin->setExternalEnvironment(external_env.toStdString());

    // Wall thickness.
    double w=std::max(readDouble(_wallThicknessValue.text()), 0.0)/1000.0;
    _plugin->setWallThickness(w);

    // Ground floor data.
    double Rsi=std::max(readDouble(_groundRsiValue.text()), 0.0);
    double R=std::max(readDouble(_groundRValue.text()), 0.0);
    double Rse=std::max(readDouble(_groundRseValue.text()), 0.0);
    _plugin->setFloorInternalSuperficialResistance(Rsi);
    _plugin->setFloorExternalSuperficialResistance(Rse);
    _plugin->setFloorResistance(R);

    // Vertical insulation.
    _plugin->includeVerticalInsulation(_enableVInsulation.isChecked());
    if (_enableVInsulation.isChecked())
    {
        double e=std::max(readDouble(_vInsThickValue.text()), 0.0)/1000.0;
        double R=std::max(readDouble(_vInsResValue.text()), 0.0);
        double L=std::max(readDouble(_vInsLenValue.text()), 0.0);

        _plugin->setVerticalInsulationThickness(e);
        _plugin->setInsulationHeight(L);
        _plugin->setVerticalInsulationResistance(R);
    }

    // Horizontal insulation.
    _plugin->includeHorizontalInsulation(_enableHInsulation.isChecked());
    if (_enableHInsulation.isChecked())
    {
        double e=std::max(readDouble(_hInsThickValue.text()), 0.0)/1000.0;
        double R=std::max(readDouble(_hInsResValue.text()), 0.0);
        double L=std::max(readDouble(_hInsLenValue.text()), 0.0);

        _plugin->setHorizontalInsulationThickness(e);
        _plugin->setInsulationWidth(L);
        _plugin->setHorizontalInsulationResistance(R);
    }

    // Crawl.
    if (type==En13370Plugin::CrawlSpace)
    {
        double ceil_r=std::max(readDouble(_crawlValue10.text()), 0.0);
        double height=std::max(readDouble(_crawlValue1.text()), 0.0);
        double ext_wall_R=std::max(readDouble(_crawlValue2.text()), 0.0);
        double area_ratio=std::max(readDouble(_crawlValue3.text()), 0.0);
        double wind_speed=std::max(readDouble(_crawlValue4.text()), 0.0);
        double wind_factor=windFactor();
        double depth=std::max(readDouble(_crawlValue6.text()), 0.0);

        double ground_wall_rsi=std::max(readDouble(_crawlValue7.text()), 0.0);
        double ground_wall_r=std::max(readDouble(_crawlValue8.text()), 0.0);
        double ground_wall_rse=std::max(readDouble(_crawlValue9.text()), 0.0);

        _plugin->setZoneCeilResistance(ceil_r);
        _plugin->setCrawlHeight(height);
        _plugin->setExternalWallResistance(ext_wall_R);
        _plugin->setOpeningsRatio(area_ratio);
        _plugin->setWindSpeed(wind_speed);
        _plugin->setWindProtectionFactor(wind_factor);
        _plugin->setUndergroundDepth(depth);

        _plugin->setWallInternalSuperficialResistance(ground_wall_rsi);
        _plugin->setWallResistance(ground_wall_r);
        _plugin->setWallExternalSuperficialResistance(ground_wall_rse);
    }

    // Basement.
    else if (type==En13370Plugin::HeatedUnderground || type==En13370Plugin::NotHeatedUnderground || type==En13370Plugin::PartiallyHeatedUnderground)
    {
        double depth=std::max(readDouble(_basementValue1.text()), 0.0);
        double wall_rsi=std::max(readDouble(_basementValue2.text()), 0.0);
        double wall_r=std::max(readDouble(_basementValue3.text()), 0.0);
        double wall_rse=std::max(readDouble(_basementValue4.text()), 0.0);

        double ceil_r=std::max(readDouble(_basementValue7.text()), 0.0);
        double mean_height=std::max(readDouble(_basementValue10.text()), 0.0);
        double air_rate=std::max(readDouble(_basementValue11.text()), 0.0);
        double ext_wall_height=std::max(readDouble(_basementValue8.text()), 0.0);
        double ext_wall_r=std::max(readDouble(_basementValue9.text()), 0.0);

        double heated_area=std::max(readDouble(_basementValue5.text()), 0.0);
        double unheated_area=std::max(readDouble(_basementValue6.text()), 0.0);

        _plugin->setUndergroundDepth(depth);
        _plugin->setWallExternalSuperficialResistance(wall_rse);
        _plugin->setWallResistance(wall_r);
        _plugin->setWallInternalSuperficialResistance(wall_rsi);
        _plugin->setZoneCeilResistance(ceil_r);
        _plugin->setMeanZoneHeight(mean_height);
        _plugin->setAirExchangeRate(air_rate);
        _plugin->setExternalWallHeight(ext_wall_height);
        _plugin->setExternalWallResistance(ext_wall_r);
        _plugin->setHeatedUndergroundArea(heated_area);
        _plugin->setNotHeatedUndergroundArea(unheated_area);
    }

    // Get flux.
    double flux=_plugin->flux();
    if (isnan(flux) || isinf(flux))
        invalidateResults();
    else
        displayResults(flux);
}

void En13370Interface::reloadMaterials()
{
    _importData=true;
    if (_delegate)
        _delegate->reloadMaterials();
    _importData=false;
}
