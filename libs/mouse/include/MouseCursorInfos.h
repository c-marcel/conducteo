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

#ifndef _Q_MOUSE_CURSOR_INFOS_H
#define _Q_MOUSE_CURSOR_INFOS_H

#include <Translatable.h>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

class MouseCursorInfos: public QWidget,
                        public Translatable
{

    Q_OBJECT

public:

    MouseCursorInfos(QWidget *parent=0);
    virtual ~MouseCursorInfos();

    void translate();

public slots:

    void unSetPosition();
    void setPosition(double x, double y);

    void unSetDimension();
    void setDimension(double dx, double dy);

protected:

    void paintEvent(QPaintEvent *event);

private:

    QVBoxLayout     _layout;
    QHBoxLayout     _layout_2;
    QVBoxLayout     _positionLayout;
    QVBoxLayout     _sizeLayout;
    QHBoxLayout     _xPositionLayout;
    QHBoxLayout     _yPositionLayout;
    QHBoxLayout     _widthLayout;
    QHBoxLayout     _heightLayout;
    QLabel          _positionTitle;
    QLabel          _xPositionTitle;
    QLabel          _xPositionValue;
    QLabel          _yPositionTitle;
    QLabel          _yPositionValue;
    QLabel          _sizeTitle;
    QLabel          _widthTitle;
    QLabel          _widthValue;
    QLabel          _heightTitle;
    QLabel          _heightValue;

};

#endif
