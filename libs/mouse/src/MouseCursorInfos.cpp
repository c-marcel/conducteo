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

#include <MouseCursorInfos.h>
#include <LinguistManager.h>
#include <QApplication>
#include <QPainter>
#include <QLocale>

MouseCursorInfos::MouseCursorInfos(QWidget *parent):
    QWidget(parent),
    _layout(this)
{
    QSpacerItem *vS1=new QSpacerItem(20, 60, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _layout.addItem(vS1);

    _layout.setContentsMargins(20, 10, 15, 0);
    _layout.setSpacing(0);

    QFont f=font();
    f.setBold(true);
    f.setPixelSize(12);

    _positionTitle.setFont(f);
    _positionTitle.setMinimumWidth(120);
    _positionLayout.addWidget(&_positionTitle);

    _xPositionTitle.setText("x :");
    _xPositionTitle.setMaximumHeight(15);
    _xPositionTitle.setMinimumWidth(50);
    _xPositionLayout.setContentsMargins(10, 0, 0, 0);
    _xPositionLayout.addWidget(&_xPositionTitle);
    _xPositionLayout.addWidget(&_xPositionValue);
    QSpacerItem *hS1=new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _xPositionLayout.addItem(hS1);
    _positionLayout.addLayout(&_xPositionLayout);

    _yPositionTitle.setText("y :");
    _yPositionTitle.setMinimumWidth(50);
    _yPositionLayout.setContentsMargins(10, 0, 0, 0);
    _yPositionLayout.addWidget(&_yPositionTitle);
    _yPositionLayout.addWidget(&_yPositionValue);
    QSpacerItem *hS2=new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _yPositionLayout.addItem(hS2);
    _positionLayout.addLayout(&_yPositionLayout);

    _sizeTitle.setFont(f);
    _sizeTitle.setMinimumWidth(120);
    _sizeLayout.addWidget(&_sizeTitle);

    QFont df = font();
    df.setPixelSize(11);

    _xPositionTitle.setFont(df);
    _xPositionValue.setFont(df);
    _yPositionTitle.setFont(df);
    _yPositionValue.setFont(df);
    _widthTitle.setFont(df);
    _widthValue.setFont(df);
    _heightTitle.setFont(df);
    _heightValue.setFont(df);

    _widthTitle.setText("Δx :");
    _widthTitle.setMinimumWidth(50);
    _widthLayout.setContentsMargins(10, 0, 0, 0);
    _widthLayout.addWidget(&_widthTitle);
    _widthLayout.addWidget(&_widthValue);
    QSpacerItem *hS3=new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _widthLayout.addItem(hS3);
    _sizeLayout.addLayout(&_widthLayout);

    _heightTitle.setText("Δy :");
    _heightTitle.setMinimumWidth(50);
    _heightLayout.setContentsMargins(10, 0, 0, 0);
    _heightLayout.addWidget(&_heightTitle);
    _heightLayout.addWidget(&_heightValue);
    QSpacerItem *hS4=new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _heightLayout.addItem(hS4);
    _sizeLayout.addLayout(&_heightLayout);

    _layout_2.addLayout(&_positionLayout);
    _layout_2.addLayout(&_sizeLayout);
    _layout.addLayout(&_layout_2);

    QSpacerItem *vS2=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS2);

    setFixedWidth(290);
    setFixedHeight(140);

    _xPositionValue.setMinimumWidth(50);
    _yPositionValue.setMinimumWidth(50);
    _widthValue.setMinimumWidth(50);
    _heightValue.setMinimumWidth(50);
    
    unSetPosition();
    unSetDimension();

    translate();
}

MouseCursorInfos::~MouseCursorInfos()
{
}

void MouseCursorInfos::paintEvent(QPaintEvent *event)
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
    QPixmap pixmap(":/images/orange-blue/mouse.png");
    int left=10;
    int top=(rec2.height()-pixmap.height())/2;
    QRect r(10+left, 10+top, pixmap.width(), pixmap.height());
    qp.drawPixmap(r, pixmap);

    // Draw text.
    QFont f=QApplication::font();

    f.setBold(true);
    f.setPixelSize(12);
    qp.setFont(f);
    qp.setPen(Qt::black);
    qp.drawText(10+2*left+pixmap.width(), 10, rec2.width()-2*left-pixmap.width(), rec2.height(), Qt::AlignVCenter, _tr("Souris"));
}

void MouseCursorInfos::setPosition(double x, double y)
{
    QLocale locale;
    _xPositionValue.setText(locale.toString(x, 'f', 3)+" m");
    _yPositionValue.setText(locale.toString(y, 'f', 3)+" m");
}

void MouseCursorInfos::setDimension(double dx, double dy)
{
    QLocale locale;
    _widthValue.setText(locale.toString(dx, 'f', 3)+" m");
    _heightValue.setText(locale.toString(dy, 'f', 3)+" m");
}

void MouseCursorInfos::unSetPosition()
{
    _xPositionValue.setText("-");
    _yPositionValue.setText("-");
}

void MouseCursorInfos::unSetDimension()
{
    _widthValue.setText("-");
    _heightValue.setText("-");
}

void MouseCursorInfos::translate()
{
    _positionTitle.setText(_tr("Position"));
    _sizeTitle.setText(_tr("Dimension"));
    update();
}
