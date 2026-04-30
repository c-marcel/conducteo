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

#include <SurfaceProperties.h>
#include <LinguistManager.h>
#include <model/Surface.h>
#include <StatesManager.h>
#include <model/Model.h>
#include <QPainter>
#include <QLocale>
#include <Log.h>

SurfaceProperties::SurfaceProperties(QWidget *parent):
    QWidget(parent),
    _layout(this)
{
    QSpacerItem *vS1=new QSpacerItem(20, 60, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _layout.addItem(vS1);

    _layout.setContentsMargins(20, 10, 15, 0);
    _layout.setSpacing(0);

    QFont defFont = font();
    defFont.setPixelSize(11);

    _fluxTitle.setFont(defFont);
    _fluxTitle.setMaximumHeight(15);
    _fluxTitle.setMinimumWidth(140);
    _fluxValue.setFont(defFont);
    _fluxLayout.addWidget(&_fluxTitle);
    _fluxLayout.addWidget(&_fluxValue);
    QSpacerItem *hS1=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _fluxLayout.addItem(hS1);
    _layout.addLayout(&_fluxLayout);

    _minTemperatureTitle.setFont(defFont);
    _minTemperatureValue.setFont(defFont);
    _minTemperatureTitle.setMaximumHeight(15);
    _minTemperatureTitle.setMinimumWidth(140);
    _minTemperatureLayout.addWidget(&_minTemperatureTitle);
    _minTemperatureLayout.addWidget(&_minTemperatureValue);
    QSpacerItem *hS2=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _minTemperatureLayout.addItem(hS2);
    _layout.addLayout(&_minTemperatureLayout);

    _maxTemperatureTitle.setFont(defFont);
    _maxTemperatureValue.setFont(defFont);
    _maxTemperatureTitle.setMaximumHeight(15);
    _maxTemperatureTitle.setMinimumWidth(140);
    _maxTemperatureLayout.addWidget(&_maxTemperatureTitle);
    _maxTemperatureLayout.addWidget(&_maxTemperatureValue);
    QSpacerItem *hS3=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _maxTemperatureLayout.addItem(hS3);
    _layout.addLayout(&_maxTemperatureLayout);

    _rhMinTitle.setFont(defFont);
    _rhMinValue.setFont(defFont);
    _rhMinTitle.setFixedHeight(20);
    _rhMinTitle.setMinimumWidth(140);
    _rhMinLayout.addWidget(&_rhMinTitle);
    _rhMinLayout.addWidget(&_rhMinValue);
    QSpacerItem *hS5 = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _rhMinLayout.addItem(hS5);
    _layout.addLayout(&_rhMinLayout);

    _rhMaxTitle.setFont(defFont);
    _rhMaxValue.setFont(defFont);
    _rhMaxTitle.setFixedHeight(20);
    _rhMaxTitle.setMinimumWidth(140);
    _rhMaxLayout.addWidget(&_rhMaxTitle);
    _rhMaxLayout.addWidget(&_rhMaxValue);
    QSpacerItem *hS6 = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _rhMaxLayout.addItem(hS6);
    _layout.addLayout(&_rhMaxLayout);

    _lengthTitle.setFont(defFont);
    _lengthValue.setFont(defFont);
    _lengthTitle.setMaximumHeight(15);
    _lengthTitle.setMinimumWidth(140);
    _lengthLayout.addWidget(&_lengthTitle);
    _lengthLayout.addWidget(&_lengthValue);
    QSpacerItem *hS4=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _lengthLayout.addItem(hS4);
    _layout.addLayout(&_lengthLayout);

    QSpacerItem *vS2=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS2);

    setFixedWidth(290);
    setFixedHeight(170);

    translate();

    _pixmap=QPixmap(":/images/orange-blue/surface.png");

    statesChanged();
}

SurfaceProperties::~SurfaceProperties()
{
}

void SurfaceProperties::setFlux(double flux)
{
    QLocale locale;
    _fluxValue.setText(locale.toString(flux, 'f', 3)+" W/m");
    update();
}

void SurfaceProperties::setMinimalTemperature(double temperature)
{
    QLocale locale;
    _minTemperatureValue.setText(locale.toString(temperature, 'f', 3)+" °C");
    update();
}

void SurfaceProperties::setMaximalTemperature(double temperature)
{
    QLocale locale;
    _maxTemperatureValue.setText(locale.toString(temperature, 'f', 3)+" °C");
    update();
}

void SurfaceProperties::setMinimalRh(double humidity)
{
    if (humidity < 0.0)
        humidity = 0.0;

    else if (humidity > 1.0)
        humidity = 1.0;

    QLocale locale;
    _rhMinValue.setText(locale.toString(humidity * 100.0, 'f', 2)+" %");

    update();
}

void SurfaceProperties::setMaximalRh(double humidity)
{
    if (humidity < 0.0)
        humidity = 0.0;

    else if (humidity > 1.0)
        humidity = 1.0;

    QLocale locale;
    _rhMaxValue.setText(locale.toString(humidity * 100.0, 'f', 2)+" %");

    update();
}

void SurfaceProperties::paintEvent(QPaintEvent *event)
{
    // Background.
    QPainter qp(this);
    QRect rec(9, 9, width()-18, height()-18);
    QBrush brush(Qt::white);
    qp.setBrush(brush);
    QPen pen;
    pen.setColor(Qt::darkGray);
    qp.setPen(pen);
    qp.drawRect(rec);

    // Header background.
    QRect rec2(10, 10, width()-19, 50);

    QColor themedColor = QColor(199, 229, 245);

    QBrush brush2(themedColor);
    qp.setBrush(brush2);
    pen.setColor(QColor(255, 255, 255, 0));
    qp.setPen(pen);
    qp.drawRect(rec2);

    // Draw image.
    QPixmap pixmap(":/images/orange-blue/surface.png");
    int left=10;
    int top=(rec2.height()-pixmap.height())/2;
    QRect r(10+left, 10+top, pixmap.width(), pixmap.height());
    qp.drawPixmap(r, pixmap);

    // Draw text.
    QFont f=font();
    f.setBold(true);
    f.setPixelSize(12);
    qp.setFont(f);

    qp.setPen(Qt::black);
    qp.drawText(10+2*left+pixmap.width(), 10, rec2.width()-2*left-pixmap.width(), rec2.height(), Qt::AlignVCenter, _tr("BoundaryCondition"));
}

void SurfaceProperties::translate()
{
    _fluxTitle.setText(_tr("HeatFluxTitle"));
    _minTemperatureTitle.setText(_tr("MinTempTitle"));
    _maxTemperatureTitle.setText(_tr("MaxTempTitle"));
    _lengthTitle.setText(_tr("LengthTitle"));
    _rhMinTitle.setText(_tr("MinHumidityTitle"));
    _rhMaxTitle.setText(_tr("MaxHumidityTitle"));

    update();
}

void SurfaceProperties::statesChanged()
{
    LOG_INFO("States changed caught.");

    bool visible=true;
    if (StatesManager::instance()->viewType() != StatesManager::DisplayResults
            && StatesManager::instance()->viewType() != StatesManager::Display1DResults
            && StatesManager::instance()->viewType() != StatesManager::DisplayCondensationResults
            && StatesManager::instance()->viewType() != StatesManager::DisplayHumidityResults)
        visible=false;
    if (StatesManager::instance()->selectedSurfaces().size()!=1)
        visible=false;
    if (StatesManager::instance()->selectedSurfaces().size()==1 && !StatesManager::instance()->selectedSurfaces().at(0)->boundaryCondition())
        visible=false;
    setVisible(visible);
    if (StatesManager::instance()->selectedSurfaces().size()==1)
        displaySurfaceData(StatesManager::instance()->selectedSurfaces().at(0));

    // Condensation.
    Model *model = StatesManager::instance()->currentModel();
    if (model && model->computationType() == Model::GlaserCondensation)
    {
        _rhMinTitle.setVisible(true);
        _rhMinValue.setVisible(true);
        _rhMaxTitle.setVisible(true);
        _rhMaxValue.setVisible(true);

        setFixedHeight(210);
    }
    else
    {
        _rhMinTitle.setVisible(false);
        _rhMinValue.setVisible(false);
        _rhMaxTitle.setVisible(false);
        _rhMaxValue.setVisible(false);

        setFixedHeight(170);
    }
}

void SurfaceProperties::displaySurfaceData(Surface *surface)
{
    if (!surface)
        return;

    setFlux(surface->flux());
    setMinimalTemperature(surface->minimalTemperature());
    setMaximalTemperature(surface->maximalTemperature());
    setMinimalRh(surface->minimalRh());
    setMaximalRh(surface->maximalRh());
    setSurfaceLength(surface->length());
}

void SurfaceProperties::setSurfaceLength(double length)
{
    QLocale locale;
    _lengthValue.setText(locale.toString(length*1000.0, 'f', 1)+" mm");
    update();
}
