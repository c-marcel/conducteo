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

#ifndef _BOUNDARY_CONDITION_LIST_H
#define _BOUNDARY_CONDITION_LIST_H

#include <databases/BoundaryConditionsObserver.h>
#include <CustomHeaderView.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

class CustomBoundaryConditionItemView;
class BoundaryCondition;

class BoundaryConditionList: public QWidget,
                             public Translatable,
                             public StatesObserver,
                             public BoundaryConditionsObserver
{

    Q_OBJECT

public:

    BoundaryConditionList(QWidget *parent=0);
    virtual ~BoundaryConditionList();

    void translate();
    void applyTheme();
    void statesChanged();
    void boundaryConditionDeleted(const std::string &id);
    void boundaryConditionAdded(const std::string &id);

public slots:

    void currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void itemDoubleClicked(QTreeWidgetItem *, int);
    void customContextMenuRequested(const QPoint &pos);
    void saveUserDatabase();

private:

    void loadStandardDatabase();
    void loadUserDatabase();
    std::vector<BoundaryCondition*> readBoundaryConditionsFromFile(const QString &filename) const;
    void addBoundaryConditionToView(BoundaryCondition *bc, QTreeWidgetItem &root);

    void expandChildren(QTreeWidgetItem &root_node, QTreeWidgetItem *item);
    void addNewBoundaryCondition(QTreeWidgetItem *item);
    void removeBoundaryConditionById(QTreeWidgetItem *item, const std::string &id);

    QVBoxLayout         _layout;
    QTreeWidget         _list;
    CustomHeaderView    _headers;
    QTreeWidgetItem     _standardBCNode;
    QTreeWidgetItem     _userBCNode;

};

#endif
