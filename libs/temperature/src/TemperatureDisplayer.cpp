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

#include <TemperatureDisplayer.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <QPainter>
#include <QLocale>
#include <Log.h>

TemperatureDisplayer::TemperatureDisplayer(QWidget *parent):
    QWidget(parent),
    _layout(this)
{
    QSpacerItem *vS1=new QSpacerItem(20, 45, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _layout.addItem(vS1);

    _layout.setContentsMargins(20, 10, 15, 0);
    _layout.setSpacing(0);

    QFont defFont = font();
    defFont.setPixelSize(11);

    _temperatureValue.setFont(defFont);
    _temperatureValue.setMinimumWidth(120);
    _temperatureValue.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _layout.addWidget(&_temperatureValue);

    setFixedWidth(290);
    setFixedHeight(110);
    
    unSetTemperature();

    translate();
    statesChanged();

    _pixmap=QPixmap(":/images/orange-blue/boundary.png");
}

TemperatureDisplayer::~TemperatureDisplayer()
{
}

void TemperatureDisplayer::paintEvent(QPaintEvent *event)
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
    int left=10;
    int top=(rec2.height()-_pixmap.height())/2;
    QRect r(10+left, 10+top, _pixmap.width(), _pixmap.height());
    qp.drawPixmap(r, _pixmap);

    // Draw text.
    QFont f=font();
    f.setBold(true);
    f.setPixelSize(12);
    qp.setFont(f);

    qp.setPen(Qt::black);
    qp.drawText(10+2*left+_pixmap.width(), 10, rec2.width()-2*left-_pixmap.width(), rec2.height(), Qt::AlignVCenter, _tr("Temperature"));
}

void TemperatureDisplayer::setTemperature(double temperature)
{
    QLocale locale;
    _temperatureValue.setText(locale.toString(temperature, 'f', 3)+" °C");
}

void TemperatureDisplayer::unSetTemperature()
{
    _temperatureValue.setText("-");
}

void TemperatureDisplayer::translate()
{
    update();
}

void TemperatureDisplayer::applyTheme()
{

    update();
}

void TemperatureDisplayer::statesChanged()
{
    LOG_INFO("States changed caught.");
    setVisible(StatesManager::instance()->viewType()==StatesManager::DisplayResults
               || StatesManager::instance()->viewType()==StatesManager::Display1DResults
               || StatesManager::instance()->viewType()==StatesManager::DisplayCondensationResults
               || StatesManager::instance()->viewType()==StatesManager::DisplayHumidityResults);
}
