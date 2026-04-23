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

#include <VerticalScale.h>
#include <StatesManager.h>
#include <QPainter>
#include <QLocale>
#include <Zoom.h>

VerticalScale::VerticalScale(QWidget *parent):
    Scale(parent)
{
    setMinimumWidth(25);
    setMaximumWidth(25);
}

VerticalScale::~VerticalScale()
{
}

void VerticalScale::paintEvent(QPaintEvent *event)
{
    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    if(_gridStep==0.0)
        updateGridStep();

    QPainter qp(this);

    double w=zoom->toPhysicalLength(height());
    int nx=w/_gridStep+2;

    double y_min=zoom->toPhysicalY(QPoint(0, height()));
    y_min=(int(y_min/_gridStep)-1)*_gridStep;

    int text_width=60;
    int text_height=15;

    QFont f = font();
    f.setPixelSize(11);
    qp.setFont(f);

    for (unsigned int i=0 ; i<nx ; i++)
    {
        qp.drawLine(2+text_height, zoom->toScreenPosition(0, y_min+i*_gridStep).y(),
                    height()-4, zoom->toScreenPosition(0, y_min+i*_gridStep).y());

        QString value=scaledValue((y_min+i*_gridStep));
        qp.save();
        qp.translate(0, zoom->toScreenPosition(0, y_min+i*_gridStep).y()+text_width/2);
        qp.rotate(-90);
        
        qp.drawText(0, 0, text_width, text_height, Qt::AlignHCenter | Qt::AlignVCenter, value);
        qp.restore();
    }

    if (_yMousePosition>=0)
    {
        QPen pen;
        pen.setColor(QColor(13, 255, 143));
        qp.setPen(pen);
        qp.drawLine(2, _yMousePosition, width()-4, _yMousePosition);
    }
}
