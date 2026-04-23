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

#ifndef _TEMPERATURE_DISPLAYER_H
#define _TEMPERATURE_DISPLAYER_H

#include <StatesObserver.h>
#include <Translatable.h>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

class TemperatureDisplayer: public QWidget,
                            public Translatable,
                            public StatesObserver
{
    Q_OBJECT
public:
    TemperatureDisplayer(QWidget *parent=0);
    virtual ~TemperatureDisplayer();

    void translate();
    void applyTheme();
    void statesChanged();

public slots:
    void setTemperature(double temperature);
    void unSetTemperature();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QVBoxLayout     _layout;
    QLabel          _temperatureValue;
    QPixmap         _pixmap;
};

#endif
