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

#ifndef _SURFACES_PROPERTIES_H
#define _SURFACES_PROPERTIES_H

#include <StatesObserver.h>
#include <Translatable.h>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

class Surface;

class SurfaceProperties: public QWidget,
                         public Translatable,
                         public StatesObserver
{
    Q_OBJECT
public:
    SurfaceProperties(QWidget *parent=0);
    virtual ~SurfaceProperties();

    void translate();
    void statesChanged();

public slots:
    void setFlux(double flux);
    void setMinimalTemperature(double temperature);
    void setMaximalTemperature(double temperature);
    void setMinimalRh(double humidity);
    void setMaximalRh(double humidity);
    void setSurfaceLength(double length);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void displaySurfaceData(Surface *surface);

    QVBoxLayout     _layout;
    QHBoxLayout     _fluxLayout;
    QHBoxLayout     _minTemperatureLayout;
    QHBoxLayout     _maxTemperatureLayout;
    QHBoxLayout     _rhMinLayout;
    QHBoxLayout     _rhMaxLayout;
    QHBoxLayout     _lengthLayout;
    QLabel          _fluxTitle;
    QLabel          _minTemperatureTitle;
    QLabel          _maxTemperatureTitle;
    QLabel          _fluxValue;
    QLabel          _minTemperatureValue;
    QLabel          _maxTemperatureValue;
    QLabel          _rhMinTitle;
    QLabel          _rhMinValue;
    QLabel          _rhMaxTitle;
    QLabel          _rhMaxValue;
    QLabel          _lengthTitle;
    QLabel          _lengthValue;
    QPixmap         _pixmap;
};

#endif
