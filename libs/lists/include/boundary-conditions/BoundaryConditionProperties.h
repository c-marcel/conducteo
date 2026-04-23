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

#ifndef _BOUNDARY_CONDITION_PROPERTIES_H
#define _BOUNDARY_CONDITION_PROPERTIES_H

#include <Translatable.h>
#include <QTableWidget>

class BoundaryCondition;

class BoundaryConditionProperties: public QTableWidget,
                                   public Translatable
{
    Q_OBJECT
public:
    BoundaryConditionProperties(QWidget *parent=0);
    virtual ~BoundaryConditionProperties();

    void setBoundaryConditionId(const QString &id);
    void setName(const QString &name);
    void setTemperature(double temperature);
    void setResistance(double resistance);
    void setRh(double rh);
    void setColor(const QColor &color);
    void setEditable(bool editable);

    void translate();

private slots:
    void cellChanged(int row, int column);
    void cellDoubleClicked(int row, int column);
    void currentSelectedColorChanged(const QColor &);

private:
    void createGrid();

    bool                _catchUserInputs;
    bool                _editable;
    BoundaryCondition  *_boundaryCondition;
    QString             _name;
    double              _temperature;
    double              _resistance;
    double              _rh;
    QColor              _color;
};

#endif
