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

#ifndef _ENVIRONMENT_LIST_H
#define _ENVIRONMENT_LIST_H

#include <CustomHeaderView.h>
#include <actions/Observer.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

class EnvironmentCustomItem;

class EnvironmentList: public QWidget,
                       public Translatable,
                       public StatesObserver,
                       public Observer
{
    Q_OBJECT
public:
    EnvironmentList(QWidget *parent=0);
    virtual ~EnvironmentList();

    void translate();
    void applyTheme();
    void statesChanged();

    void updateDescriptions();

    void changed();
    void modelAltered();
    void modelResultsAltered();

public slots:
    void customContextMenuRequested(const QPoint &pos);

private slots:
    void currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);

private:
    QVBoxLayout         _layout;
    QTreeWidget         _list;
    CustomHeaderView    _headers;
    QTreeWidgetItem     _environmentsNode;

    QColor              _environment1Color;
    QColor              _environment2Color;
    QColor              _environment3Color;
    QColor              _environment4Color;

    EnvironmentCustomItem  *_item1;
    EnvironmentCustomItem  *_item2;
    EnvironmentCustomItem  *_item3;
    EnvironmentCustomItem  *_item4;
};

#endif
