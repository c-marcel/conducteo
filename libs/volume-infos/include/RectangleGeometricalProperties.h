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

#ifndef _RECTANGLE_GEOMETRICAL_PROPERTIES_H
#define _RECTANGLE_GEOMETRICAL_PROPERTIES_H

#include <Translatable.h>
#include <QTableWidget>

class RectangleGeometricalProperties: public QTableWidget,
                                      public Translatable
{

    Q_OBJECT

public:

    RectangleGeometricalProperties(QWidget *parent=0);
    virtual ~RectangleGeometricalProperties();

    void setData(const double x, const double y, const double width, const double height);

    void translate();

signals:

    void dataXChanged(double x);
    void dataYChanged(double y);
    void dataWidthChanged(double width);
    void dataHeightChanged(double height);

private slots:

    void onCellChanged(int row, int column);

private:

    bool    _catchUserInputs;

};

#endif
