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

#include <environments/EnvironmentList.h>
#include <actions/environments/SetEnvironmentDescription.h>
#include <environments/EnvironmentCustomItem.h>
#include <actions/ActionsManager.h>
#include <databases/Environments.h>
#include <physics/Environment.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <QInputDialog>
#include <QMenu>

EnvironmentList::EnvironmentList(QWidget *parent):
    QWidget(parent),
    _layout(this),
    _headers(Qt::Horizontal),
    _environment1Color(204, 12, 12),
    _environment2Color(111, 242, 111),
    _environment3Color(0, 136, 255),
    _environment4Color(231, 237, 40),
    _item1(0),
    _item2(0),
    _item3(0),
    _item4(0)
{
    setFixedWidth(290);
    setFixedHeight(230);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Environments list.
    _list.setHeader(&_headers);
    _list.setContextMenuPolicy(Qt::CustomContextMenu);

    QFont f = font();
    f.setPixelSize(12);
    _list.setFont(f);

    _headers.resizeSection(0, 200);
    _headers.setBaseSize(200, 50);
    _layout.addWidget(&_list);

    // Add root node.
    _environmentsNode.setFlags(Qt::ItemIsEnabled);
    _environmentsNode.setSizeHint(0, QSize(150, 25));

    _list.addTopLevelItem(&_environmentsNode);
    _environmentsNode.setExpanded(true);
    _list.setItemsExpandable(false);

    // Add environments.
    EnvironmentCustomItem *environment0=new EnvironmentCustomItem;
    environment0->setTitle(_tr("Ambiance non définie"));
    environment0->setId("");
    environment0->setColor(Qt::black);
    _environmentsNode.addChild(environment0);
    _list.setItemWidget(environment0, 0, environment0);

    _item1 = new EnvironmentCustomItem;
    _item1->setColor(_environment1Color);
    _item1->setId("1");
    _environmentsNode.addChild(_item1);
    _list.setItemWidget(_item1, 0, _item1);

    _item2 = new EnvironmentCustomItem;
    _item2->setColor(_environment2Color);
    _item2->setId("2");
    _environmentsNode.addChild(_item2);
    _list.setItemWidget(_item2, 0, _item2);

    _item3 = new EnvironmentCustomItem;
    _item3->setColor(_environment3Color);
    _item3->setId("3");
    _environmentsNode.addChild(_item3);
    _list.setItemWidget(_item3, 0, _item3);

    _item4 = new EnvironmentCustomItem;
    _item4->setColor(_environment4Color);
    _item4->setId("4");
    _environmentsNode.addChild(_item4);
    _list.setItemWidget(_item4, 0, _item4);

    connect(&_list, &QTreeWidget::currentItemChanged, this, &EnvironmentList::currentItemChanged);
    connect(&_list,
            &QTreeWidget::customContextMenuRequested,
            this,
            qOverload<const QPoint &>(&EnvironmentList::customContextMenuRequested));

    translate();
    applyTheme();
    statesChanged();
    updateDescriptions();

    ActionsManager::instance()->registerObserver(this);
}

EnvironmentList::~EnvironmentList()
{
    ActionsManager::instance()->unregisterObserver(this);
}

void EnvironmentList::currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *)
{
    EnvironmentCustomItem *env=dynamic_cast<EnvironmentCustomItem*>(item);
    if (env)
        StatesManager::instance()->setCurrentEnvironmentId(env->id().toStdString());
    else
        StatesManager::instance()->setCurrentEnvironmentId("");
}

void EnvironmentList::translate()
{
    _headers.setTitle(_tr("Ambiances thermiques"));
    _environmentsNode.setText(0, _tr("Ambiances thermiques"));
    update();
}

void EnvironmentList::applyTheme()
{
    _headers.setIcon(QPixmap(":/images/orange-blue/environments.png"));

    QColor themedColor = QColor(199, 229, 245);

    _headers.setBackgroundColor(themedColor);

    update();
}

void EnvironmentList::statesChanged()
{
    setVisible(StatesManager::instance()->interfaceMode()==StatesManager::SetEnvironments);
}

void EnvironmentList::updateDescriptions()
{
    Environment *env1 = Environments::instance()->environmentFromName("1");
    if (env1 && _item1)
        _item1->setTitle(env1->description().c_str());

    Environment *env2 = Environments::instance()->environmentFromName("2");
    if (env2 && _item2)
        _item2->setTitle(env2->description().c_str());

    Environment *env3 = Environments::instance()->environmentFromName("3");
    if (env3 && _item3)
        _item3->setTitle(env3->description().c_str());

    Environment *env4 = Environments::instance()->environmentFromName("4");
    if (env4 && _item4)
        _item4->setTitle(env4->description().c_str());
}

void EnvironmentList::changed()
{
    // Do nothing.
}

void EnvironmentList::modelAltered()
{
    updateDescriptions();
}

void EnvironmentList::modelResultsAltered()
{
    // Do nothing.
}

void EnvironmentList::customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = _list.itemAt(pos);
    if (!item)
        return;

    EnvironmentCustomItem *envItem = dynamic_cast<EnvironmentCustomItem*>(item);
    if (!envItem)
        return;

    if (envItem->id() == "")
        return;

    Environment *env = Environments::instance()->environmentFromName(envItem->id().toStdString());
    if (!env)
        return;

    QMenu menu(this);

    QPoint globalPos=_list.mapToGlobal(pos);
    globalPos.setY(globalPos.y() + _headers.height());

    QAction *rename = menu.addAction(_tr("Renommer"));
    QAction *choice = menu.exec(globalPos);

    if (choice != rename)
        return;

    QString title = envItem->title();

    QString text = QInputDialog::getText(this, _tr("Renommer une ambiance thermique"), _tr("Nom :"), QLineEdit::Normal, title);
    if (text.isEmpty())
        return;

    SetEnvironmentDescription *action = new SetEnvironmentDescription(env->name(), text.toStdString());
    ActionsManager::instance()->exec(action);

    updateDescriptions();
}
