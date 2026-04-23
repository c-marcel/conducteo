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

#include <Scale.h>
#include <StatesManager.h>
#include <QPainter>
#include <QLocale>
#include <Zoom.h>
#include <cmath>

Scale::Scale(QWidget *parent):
    QWidget(parent),
    _minScreenGridStep(100),
    _gridStep(0.0),
    _xMousePosition(-1),
    _yMousePosition(-1)
{
    connect(StatesManager::instance()->zoom(), &Zoom::zoomChanged, this, &Scale::update, Qt::UniqueConnection);
}

Scale::~Scale()
{
}

void Scale::update()
{
    updateGridStep();
    repaint();
}

void Scale::updateGridStep()
{
    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    // Get physical minimal grid step.
    double min_step=zoom->toPhysicalLength(_minScreenGridStep);
    
    // Get power.
    int p=int(-log10(min_step)+1.0);

    // Compute ranged value.
    double v=min_step/pow(0.1, p);

    // Compute dx.
    _gridStep=pow(0.1, p)*5.0;
}

QString Scale::scaledValue(const double value)
{
    QLocale locale;
    if (fabs(value)>=1)
        return locale.toString(value, 'f', 2)+ " m";
    if (fabs(value)>0.01)
        return locale.toString(value*100, 'f', 2)+ " cm";
    if (fabs(value)>0.001)
        return locale.toString(value*1000, 'f', 2)+ " mm";
    return locale.toString(value, 'f', 2)+ " m";
}

void Scale::mouseScreenPositionChanged(int x, int y)
{
    _xMousePosition=x;
    _yMousePosition=y;
    repaint();
}

void Scale::statesChanged()
{
    setVisible(StatesManager::instance()->viewType()!=StatesManager::DisplayReport);
}
