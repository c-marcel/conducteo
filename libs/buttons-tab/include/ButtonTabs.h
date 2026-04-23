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

#ifndef _BUTTON_TABS_H
#define _BUTTON_TABS_H

#include <StatesObserver.h>
#include <Translatable.h>
#include <MenuButton.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QMenu>

class ButtonTabs: public QWidget,
                  public Translatable,
                  public StatesObserver
{

    Q_OBJECT

public:

    ButtonTabs(QWidget *parent=0);
    virtual ~ButtonTabs();

    void translate();
    void applyTheme();

    void initResultsState();

    void statesChanged();

private slots:

    void modelButtonClicked();
    void model1dButton2Clicked();
    void resultsButtonClicked();
    void reportButtonClicked();
    void meshButtonClicked();

    void results2dRequested(bool);
    void results1dRequested(bool);

    void glaserHumidityRequested(bool);
    void glaserCondensationRequested(bool);
    void glaserTemperatureRequested(bool);

private:
    enum GlaserViewType
    {
        GlaserHumidity,
        GlaserCondensation,
        GlaserTemperature
    };

    void show2dResults();
    void show1dResults();

    void showGlaserHumidity();
    void showGlaserCondensation();
    void showGlaserTemperature();

    QHBoxLayout     _layout;
    QPushButton     _modelButton;
    QPushButton     _1DModelButton;
    MenuButton      _resultsButton;
    QPushButton     _meshButton;
    QPushButton     _reportButton;
    QMenu           _resultsMenu;
    QMenu           _condensationMenu;
    QAction        *_display2d;
    QAction        *_display1d;
    QAction        *_glaserTemperatures;
    QAction        *_glaserHumidity;
    QAction        *_glaserCondensation;
    bool            _display2dType;
    GlaserViewType  _glaserViewType;
};

#endif
