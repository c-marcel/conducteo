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

#include <HorizontalScale.h>
#include <StatesManager.h>
#include <QPainter>
#include <QLocale>
#include <Zoom.h>

HorizontalScale::HorizontalScale(QWidget *parent):
   Scale(parent)
{
    setMinimumHeight(25);
    setMaximumHeight(25);
}

HorizontalScale::~HorizontalScale()
{
}

void HorizontalScale::paintEvent(QPaintEvent *event)
{
    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    if(_gridStep==0.0)
        updateGridStep();

    QPainter qp(this);

    double w=zoom->toPhysicalLength(width());
    int nx=w/_gridStep+2;

    double x_min=zoom->toPhysicalX(QPoint(0, 0));
    x_min=(int(x_min/_gridStep)-1)*_gridStep;

    int text_width=60;
    int text_height=15;

    QFont f = font();
    f.setPixelSize(11);
    qp.setFont(f);

    for (unsigned int i=0 ; i<nx ; i++)
    {
        qp.drawLine(zoom->toScreenPosition(x_min+i*_gridStep, 0).x(), 2+text_height,
                    zoom->toScreenPosition(x_min+i*_gridStep, 0).x(), height()-4);

        QString value=scaledValue(x_min+i*_gridStep);
        qp.drawText(zoom->toScreenPosition(x_min+i*_gridStep, 0).x()-text_width/2, 0,
                    text_width, text_height, Qt::AlignHCenter | Qt::AlignVCenter, value);
    }

    if (_xMousePosition>=0)
    {
        QPen pen;
        pen.setColor(QColor(13, 255, 143));
        qp.setPen(pen);
        qp.drawLine(_xMousePosition, 2, _xMousePosition, height()-4);
    }
}
