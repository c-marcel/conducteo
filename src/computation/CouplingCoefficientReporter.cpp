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

#include <computation/CouplingCoefficientReporter.h>
#include <QPainterPath>
#include <QPainter>
#include <QLocale>

CouplingCoefficientReporter::CouplingCoefficientReporter(QWidget *parent):
    QWidget(parent)
{
    setFixedHeight(25);
    setFixedWidth(170);
}

CouplingCoefficientReporter::~CouplingCoefficientReporter()
{
}

void CouplingCoefficientReporter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QLocale locale;

    QTextOption o;
    o.setAlignment(Qt::AlignVCenter);

    QFont f=painter.font();
    f.setPixelSize(11);
    painter.setFont(f);

    QFont small_font=f;
    small_font.setPixelSize(7);

    int left_margin=10;
    int v_margins=4;
    int box_width=30;

    QPainterPath path1;
    path1.moveTo(left_margin, v_margins);
    path1.lineTo(left_margin, height()-v_margins);
    path1.lineTo(left_margin+box_width, v_margins);
    path1.lineTo(left_margin, v_margins);

    QPainterPath path2;
    path2.moveTo(left_margin, height()-v_margins);
    path2.lineTo(left_margin+box_width, height()-v_margins);
    path2.lineTo(left_margin+box_width, v_margins);
    path2.lineTo(left_margin, height()-v_margins);

    painter.setBrush(_color1);
    painter.setPen(Qt::transparent);
    painter.drawPath(path1);

    painter.setBrush(_color2);
    painter.setPen(Qt::transparent);
    painter.drawPath(path2);

    painter.setPen(Qt::black);

    QRect entry_rect(50, 0, width()-50, height());
    QString title="ψ   : "+locale.toString(_coefficient, 'f', 3)+" W/(m.°C)";
    painter.drawText(entry_rect, title, o);

    painter.setFont(small_font);
    QRect entry_rect_indice(56, 17, width()-60, 20);
    QString indice=_id1+_id2;
    painter.drawText(entry_rect_indice, indice);
}

void CouplingCoefficientReporter::setFirstEnvironment(const QString &id)
{
    _id1=id;
}

void CouplingCoefficientReporter::setSecondEnvironment(const QString &id)
{
    _id2=id;
}

void CouplingCoefficientReporter::setCoefficient(double value)
{
    _coefficient=value;
}

void CouplingCoefficientReporter::setFirstEnvColor(const QColor &color)
{
    _color1=color;
}

void CouplingCoefficientReporter::setSecondEnvColor(const QColor &color)
{
    _color2=color;
}
