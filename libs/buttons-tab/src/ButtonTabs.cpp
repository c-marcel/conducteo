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

#include <ButtonTabs.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <model/Model.h>
#include <Log.h>

ButtonTabs::ButtonTabs(QWidget *parent):
    QWidget(parent),
    _layout(this),
    _display2dType(true),
    _glaserViewType(GlaserHumidity)
{
    _layout.setContentsMargins(0, 7, 0, 0);
    
    _modelButton.setCheckable(true);
    _layout.addWidget(&_modelButton);

    _1DModelButton.setCheckable(true);
    _layout.addWidget(&_1DModelButton);

    _resultsButton.setCheckable(true);

    _display2d=_resultsMenu.addAction(_tr(""));
    _display1d=_resultsMenu.addAction(_tr(""));

    _display2d->setCheckable(true);
    _display2d->setChecked(true);
    _display1d->setCheckable(true);
    _display1d->setChecked(false);

    _glaserHumidity     = _condensationMenu.addAction(_tr(""));
    _glaserCondensation = _condensationMenu.addAction(_tr(""));
    _glaserTemperatures = _condensationMenu.addAction(_tr(""));

    _glaserHumidity->setCheckable(true);
    _glaserCondensation->setCheckable(true);
    _glaserTemperatures->setCheckable(true);

    _glaserHumidity->setChecked(true);
    _glaserCondensation->setChecked(false);
    _glaserTemperatures->setChecked(false);
    
    _layout.addWidget(&_resultsButton);

    _meshButton.setCheckable(true);
    _layout.addWidget(&_meshButton);

    _reportButton.setCheckable(true);
    _layout.addWidget(&_reportButton);

    QSpacerItem *hS1=new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _layout.addItem(hS1);

    connect(&_modelButton, &QAbstractButton::clicked, this, &ButtonTabs::modelButtonClicked);
    connect(&_1DModelButton, &QAbstractButton::clicked, this, &ButtonTabs::model1dButton2Clicked);
    connect(&_resultsButton, &QAbstractButton::clicked, this, &ButtonTabs::resultsButtonClicked);
    connect(&_reportButton, &QAbstractButton::clicked, this, &ButtonTabs::reportButtonClicked);
    connect(&_meshButton, &QAbstractButton::clicked, this, &ButtonTabs::meshButtonClicked);

    connect(_display2d, &QAction::triggered, this, &ButtonTabs::results2dRequested);
    connect(_display1d, &QAction::triggered, this, &ButtonTabs::results1dRequested);

    connect(_glaserTemperatures, &QAction::triggered, this, &ButtonTabs::glaserTemperatureRequested);
    connect(_glaserHumidity, &QAction::triggered, this, &ButtonTabs::glaserHumidityRequested);
    connect(_glaserCondensation,
            &QAction::triggered,
            this,
            &ButtonTabs::glaserCondensationRequested);

    translate();
    applyTheme();
    statesChanged();
}

ButtonTabs::~ButtonTabs()
{
}

void ButtonTabs::modelButtonClicked()
{
    LOG_INFO("Model button clicked.");
    StatesManager::instance()->setViewType(StatesManager::DisplayModel);
}

void ButtonTabs::model1dButton2Clicked()
{
    LOG_INFO("Model 1d button clicked.");
    StatesManager::instance()->setViewType(StatesManager::Display1DModel);
}

void ButtonTabs::resultsButtonClicked()
{
    LOG_INFO("Results button clicked.");

    Model *model = StatesManager::instance()->currentModel();
    if (model)
    {
        Model::ComputationType type = model->computationType();

        // Condensation mode.
        if (type == Model::GlaserCondensation)
        {
            if (_glaserViewType == GlaserHumidity)
                StatesManager::instance()->setViewType(StatesManager::DisplayHumidityResults);
            else if (_glaserViewType == GlaserCondensation)
                StatesManager::instance()->setViewType(StatesManager::DisplayCondensationResults);
            else
                StatesManager::instance()->setViewType(StatesManager::DisplayResults);
        }

        // All other modes (thermal).
        else
        {
            if (_display2dType)
                StatesManager::instance()->setViewType(StatesManager::DisplayResults);
            else
                StatesManager::instance()->setViewType(StatesManager::Display1DResults);
        }
    }
}

void ButtonTabs::initResultsState()
{
    _display2dType  = true;
    _glaserViewType = GlaserHumidity;
}

void ButtonTabs::reportButtonClicked()
{
    LOG_INFO("Report button clicked.");
    StatesManager::instance()->setViewType(StatesManager::DisplayReport);
}

void ButtonTabs::meshButtonClicked()
{
    LOG_INFO("Mesh button clicked.");
    StatesManager::instance()->setViewType(StatesManager::DisplayMesh);
}

void ButtonTabs::translate()
{
    _modelButton.setText(_tr("ModelButton"));
    _1DModelButton.setText(_tr("Model1DButton"));
    _resultsButton.setTitle(_tr("ResultsButton"));
    _reportButton.setText(_tr("ReportButton"));
    _meshButton.setText(_tr("MeshButton"));

    _display2d->setText(_tr("Display2DText"));
    _display1d->setText(_tr("Display1DText"));

    _glaserTemperatures->setText(_tr("GlaserTemperatures"));
    _glaserHumidity->setText(_tr("GlaserHumidity"));
    _glaserCondensation->setText(_tr("GlaserCondensation"));
}

void ButtonTabs::applyTheme()
{
    _modelButton.setIcon(QIcon(":/images/orange-blue/psi.png"));
    _modelButton.setIconSize(QSize(24, 24));
    _1DModelButton.setIcon(QIcon(":/images/orange-blue/mesh.png"));
    _1DModelButton.setIconSize(QSize(24, 24));
    _resultsButton.setIcon(QIcon(":/images/orange-blue/temperatures.png"));
    _resultsButton.setIconSize(QSize(24, 24));
    _reportButton.setIcon(QIcon(":/images/orange-blue/report.png"));
    _reportButton.setIconSize(QSize(24, 24));
    _meshButton.setIcon(QIcon(":/images/orange-blue/numeric.png"));
    _meshButton.setIconSize(QSize(24, 24));
}

void ButtonTabs::statesChanged()
{
    LOG_INFO("States changed caught.");

    _resultsButton.setVisible(StatesManager::instance()->simulationState()==StatesManager::Done);
    _reportButton.setVisible(StatesManager::instance()->simulationState()==StatesManager::Done);
    _meshButton.setVisible(StatesManager::instance()->simulationState()==StatesManager::Done);

    Model *model=StatesManager::instance()->currentModel();
    if (model)
        _1DModelButton.setVisible(model
                                  && (model->computationType() != Model::Flux)
                                  && (model->computationType() != Model::EquivalentThermalConductivity)
                                  && (model->computationType() != Model::GlaserCondensation));

    _modelButton.setChecked(StatesManager::instance()->viewType()==StatesManager::DisplayModel);
    _1DModelButton.setChecked(StatesManager::instance()->viewType()==StatesManager::Display1DModel);
    _resultsButton.setChecked(StatesManager::instance()->viewType()==StatesManager::DisplayResults || StatesManager::instance()->viewType()==StatesManager::Display1DResults);
    _reportButton.setChecked(StatesManager::instance()->viewType()==StatesManager::DisplayReport);
    _meshButton.setChecked(StatesManager::instance()->viewType()==StatesManager::DisplayMesh);

    if (StatesManager::instance()->simulationState()==StatesManager::Idle)
    {
        _display2dType=true;
        _display2d->setChecked(true);
        _display1d->setChecked(false);
    }

    if (model && model->computationType() == Model::ThermalBridge)
        _resultsButton.installMenu(&_resultsMenu);
    else if (model && model->computationType() == Model::ThermalTransmission)
        _resultsButton.installMenu(&_resultsMenu);
    else if (model && model->computationType() == Model::Flux)
        _resultsButton.installMenu(0);
    else if (model && model->computationType() == Model::EquivalentThermalConductivity)
        _resultsButton.installMenu(0);
    else if (model && model->computationType() == Model::GlaserCondensation)
        _resultsButton.installMenu(&_condensationMenu);

    // HACK: Result button x-offset.
    if (model && model->computationType() == Model::GlaserCondensation)
    {
#ifdef __APPLE__
        _resultsButton.setMargins(170, -8);
#endif

#ifdef _WIN32
        _resultsButton.setMargins(200, -5);
#endif
    }

    else
    {
#ifdef __APPLE__
        _resultsButton.setMargins(-80, -8);
#endif

#ifdef _WIN32
        _resultsButton.setMargins(0, -5);
#endif
    }
}

void ButtonTabs::results2dRequested(bool c)
{
    LOG_INFO("2d results requested.");
    show2dResults();
}

void ButtonTabs::results1dRequested(bool c)
{
    LOG_INFO("1d results requested.");
    show1dResults();
}

void ButtonTabs::glaserHumidityRequested(bool)
{
    LOG_INFO("Glaser humidity requested.");
    showGlaserHumidity();
}

void ButtonTabs::glaserCondensationRequested(bool)
{
    LOG_INFO("Glaser condensation requested.");
    showGlaserCondensation();
}

void ButtonTabs::glaserTemperatureRequested(bool)
{
    LOG_INFO("Glaser temperature requested.");
    showGlaserTemperature();
}

void ButtonTabs::show2dResults()
{
    _display2dType=true;
    _display2d->setChecked(true);
    _display1d->setChecked(false);
    resultsButtonClicked();
}

void ButtonTabs::show1dResults()
{
    _display2dType=false;
    _display2d->setChecked(false);
    _display1d->setChecked(true);
    resultsButtonClicked();
}

void ButtonTabs::showGlaserHumidity()
{
    _glaserViewType = GlaserHumidity;

    _glaserHumidity->setChecked(true);
    _glaserCondensation->setChecked(false);
    _glaserTemperatures->setChecked(false);

    resultsButtonClicked();
}

void ButtonTabs::showGlaserCondensation()
{
    _glaserViewType = GlaserCondensation;

    _glaserHumidity->setChecked(false);
    _glaserCondensation->setChecked(true);
    _glaserTemperatures->setChecked(false);

    resultsButtonClicked();
}

void ButtonTabs::showGlaserTemperature()
{
    _glaserViewType = GlaserTemperature;

    _glaserHumidity->setChecked(false);
    _glaserCondensation->setChecked(false);
    _glaserTemperatures->setChecked(true);

    resultsButtonClicked();
}
