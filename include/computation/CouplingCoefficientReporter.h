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

#ifndef _COUPLING_COEFFICIENT_REPORTER_H
#define _COUPLING_COEFFICIENT_REPORTER_H

#include <QWidget>

class CouplingCoefficientReporter: public QWidget
{
public:
    CouplingCoefficientReporter(QWidget *parent=0);
    virtual ~CouplingCoefficientReporter();

    void setFirstEnvironment(const QString &id);
    void setSecondEnvironment(const QString &id);
    void setCoefficient(double value);
    void setFirstEnvColor(const QColor &color);
    void setSecondEnvColor(const QColor &color);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QString     _id1;
    QString     _id2;
    double      _coefficient;
    QColor      _color1;
    QColor      _color2;
};

#endif
