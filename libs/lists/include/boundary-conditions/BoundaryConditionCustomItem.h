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

#ifndef _BOUNDARY_CONDITION_CUSTOM_ITEM_H
#define _BOUNDARY_CONDITION_CUSTOM_ITEM_H

#include <actions/Observer.h>
#include <QTreeWidgetItem>
#include <Translatable.h>
#include <QWidget>

class BoundaryConditionProperties;
class BoundaryCondition;

class BoundaryConditionCustomItem: public QWidget,
                                   public QTreeWidgetItem,
                                   public Observer,
                                   public Translatable
{
    Q_OBJECT
public:
    BoundaryConditionCustomItem();
    virtual ~BoundaryConditionCustomItem();

    void translate();

    void setBoundaryCondition(BoundaryCondition *bc);
    void setTitle(const QString &title);
    void setColor(const QColor &color);
    void setTemperature(double temperature);
    void setResistance(double resistance);
    void setRh(double rh);
    void setEditable(bool editable);

    QString id() const;
    QString title() const;

    void expandItem(QTreeWidgetItem *item);
    void changed();
    void modelAltered();
    void modelResultsAltered();

    bool operator<(const QTreeWidgetItem &other) const;

signals:
    void saveDatabaseRequested();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    void setExpanded(bool expanded);
    void updateTooltip();

    QString                         _id;
    QString                         _title;
    QString                         _translatedTitle;
    QColor                          _color;
    double                          _temperature;
    double                          _resistance;
    double                          _rh;
    BoundaryConditionProperties    *_properties;
    bool                            _expanded;
    bool                            _editable;
};

#endif
