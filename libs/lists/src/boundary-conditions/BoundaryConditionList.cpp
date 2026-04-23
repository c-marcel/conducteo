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

#include <boundary-conditions/BoundaryConditionList.h>
#include <actions/boundary-conditions/CreateBoundaryCondition.h>
#include <actions/boundary-conditions/DeleteBoundaryCondition.h>
#include <boundary-conditions/BoundaryConditionCustomItem.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>
#include <actions/ActionsManager.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <tools/ToolBox.h>
#include <LocalData.h>
#include <tinyxml.h>
#include <fstream>
#include <QMenu>
#include <QFile>
#include <Log.h>

BoundaryConditionList::BoundaryConditionList(QWidget *parent):
    QWidget(parent),
    _layout(this),
    _headers(Qt::Horizontal)
{
    setFixedWidth(290);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // BC list.
    _list.setHeader(&_headers);
    _list.setContextMenuPolicy(Qt::CustomContextMenu);

    QFont f = font();
    f.setPixelSize(12);
    _list.setFont(f);

    _headers.resizeSection(0, 200);
    _headers.setBaseSize(200, 50);
    _layout.addWidget(&_list);

    // Add root nodes.
    _standardBCNode.setFlags(Qt::ItemIsEnabled);
    _standardBCNode.setSizeHint(0, QSize(150, 25));

    _userBCNode.setFlags(Qt::ItemIsEnabled);
    _userBCNode.setSizeHint(0, QSize(150, 25));

    _list.addTopLevelItem(&_standardBCNode);
    _list.addTopLevelItem(&_userBCNode);

    _standardBCNode.setExpanded(true);
    _userBCNode.setExpanded(true);

    connect(&_list,
            &QTreeWidget::currentItemChanged,
            this,
            &BoundaryConditionList::currentItemChanged);
    connect(&_list,
            &QTreeWidget::itemDoubleClicked,
            this,
            &BoundaryConditionList::itemDoubleClicked);
    connect(&_list,
            &QTreeView::customContextMenuRequested,
            this,
            &BoundaryConditionList::customContextMenuRequested);

    translate();
    applyTheme();
    statesChanged();

    setMinimumHeight(250);

    // Load databases.
    loadStandardDatabase();
    loadUserDatabase();

    BoundaryConditions::instance()->registerObserver(this);
}

BoundaryConditionList::~BoundaryConditionList()
{
}

void BoundaryConditionList::customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item=_list.itemAt(pos);
    
    // Add menu.
    if (item==&_userBCNode)
    {
        QMenu menu(this);
        QPoint globalPos=_list.mapToGlobal(pos);
        globalPos.setY(globalPos.y()+_headers.height());
        QAction *add=menu.addAction(_tr("Ajouter une condition à la limite"));
        QAction *choice=menu.exec(globalPos);
        if (choice==add)
            addNewBoundaryCondition(&_userBCNode);
    }

    // Remove/modify menu.
    else if (item && item->parent()==&_userBCNode)
    {
        QMenu menu(this);
        QPoint globalPos=_list.mapToGlobal(pos);
        globalPos.setY(globalPos.y()+_headers.height());
        QAction *remove=menu.addAction(_tr("Supprimer la condition à la limite"));
        QAction *modify=menu.addAction(_tr("Modifier la condition à la limite"));
        QAction *choice=menu.exec(globalPos);
        if (choice==remove)
        {
            BoundaryConditionCustomItem *m=dynamic_cast<BoundaryConditionCustomItem*>(item);
            if (!m)
                return;

            QString id=m->id();

            BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(id.toStdString());
            if (!bc)
                return;

            DeleteBoundaryCondition *action=new DeleteBoundaryCondition(bc);
            ActionsManager::instance()->exec(action);
        }
        else if (choice==modify)
        {
            itemDoubleClicked(item, 0);
        }
    }

    // Read-only boundary conditions.
    else if (item && item->parent()==&_standardBCNode)
    {
        QMenu menu(this);
        QPoint globalPos=_list.mapToGlobal(pos);
        globalPos.setY(globalPos.y()+_headers.height());
        QAction *modify=menu.addAction(_tr("Déplier la condition à la limite"));
        QAction *choice=menu.exec(globalPos);
        
        if (choice==modify)
        {
            itemDoubleClicked(item, 0);
        }
    }
}

void BoundaryConditionList::currentItemChanged(QTreeWidgetItem *it, QTreeWidgetItem *)
{
    BoundaryConditionCustomItem *item=dynamic_cast<BoundaryConditionCustomItem*>(it);
    if (item)
        StatesManager::instance()->setCurrentBoundaryConditionId(item->id().toStdString());
    else
        StatesManager::instance()->setCurrentBoundaryConditionId("");
}

void BoundaryConditionList::itemDoubleClicked(QTreeWidgetItem *item, int)
{
    BoundaryConditionCustomItem *bc=dynamic_cast<BoundaryConditionCustomItem*>(item);
    if (!bc)
        return;

    expandChildren(_standardBCNode, item);
    expandChildren(_userBCNode, item);
    _list.resizeColumnToContents(0);
}

void BoundaryConditionList::expandChildren(QTreeWidgetItem &root_node, QTreeWidgetItem *item)
{
    for (int i=0 ; i<root_node.childCount() ; i++)
    {
        BoundaryConditionCustomItem *bc=dynamic_cast<BoundaryConditionCustomItem*>(root_node.child(i));
        if (!bc)
            continue;
        bc->expandItem(item);
    }
}

void BoundaryConditionList::translate()
{
    _headers.setTitle(_tr("Conditions limites"));
    _standardBCNode.setText(0, _tr("Bibliothèque standard"));
    _userBCNode.setText(0, _tr("Bibliothèque personnelle"));
}

void BoundaryConditionList::applyTheme()
{
    _headers.setIcon(QPixmap(":/images/orange-blue/boundary.png"));

    QColor themedColor  = QColor(199, 229, 245);

    _headers.setBackgroundColor(themedColor);

    update();
}

void BoundaryConditionList::statesChanged()
{
    LOG_INFO("States changed caught.");
    setVisible(StatesManager::instance()->interfaceMode()==StatesManager::SetPhysics);
}

void BoundaryConditionList::boundaryConditionDeleted(const std::string &id)
{
    removeBoundaryConditionById(&_userBCNode, id);
    saveUserDatabase();
}

void BoundaryConditionList::boundaryConditionAdded(const std::string &id)
{
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(id);
    if (!bc)
        return;

    addBoundaryConditionToView(bc, _userBCNode);
    saveUserDatabase();
}

void BoundaryConditionList::saveUserDatabase()
{
    QByteArray xml="<boundary-conditions>";
    for (int i=0 ; i<_userBCNode.childCount() ; i++)
    {
        QTreeWidgetItem *item=_userBCNode.child(i);
        BoundaryConditionCustomItem *bc=dynamic_cast<BoundaryConditionCustomItem*>(item);
        if (!bc)
            continue;

        QString id=bc->id();

        BoundaryCondition *m=BoundaryConditions::instance()->bcFromId(id.toStdString());
        if (!m)
            continue;
        xml+=m->serialize().c_str();
    }
    xml+="</boundary-conditions>";
    
    QString filename=LocalData::instance()->getLocalPath().c_str();

    filename = filename + "/boundaries-v3.db";

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(xml);
    file.close();
}

void BoundaryConditionList::loadStandardDatabase()
{
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId("");
    if (bc)
        addBoundaryConditionToView(bc, _standardBCNode);

    bc=BoundaryConditions::instance()->bcFromId("BEB3A51D-800D-42A4-94E1-CA053B844276");
    if (bc)
        addBoundaryConditionToView(bc, _standardBCNode);

    bc=BoundaryConditions::instance()->bcFromId("9D7B0B22-6578-42EE-9989-FAF7C0B1B37B");
    if (bc)
        addBoundaryConditionToView(bc, _standardBCNode);

    bc=BoundaryConditions::instance()->bcFromId("9DRB0B67-6578-42EE-CAF1-FAF7C0B1B37B");
    if (bc)
        addBoundaryConditionToView(bc, _standardBCNode);

    bc=BoundaryConditions::instance()->bcFromId("10F097B9-6864-42CC-9E9B-C7BBD07371B0");
    if (bc)
        addBoundaryConditionToView(bc, _standardBCNode);

    bc=BoundaryConditions::instance()->bcFromId("B33C4F4E-2749-4A14-A022-FCEB5F4A713C");
    if (bc)
        addBoundaryConditionToView(bc, _standardBCNode);
    
    _standardBCNode.sortChildren(0, Qt::AscendingOrder);
}

void BoundaryConditionList::loadUserDatabase()
{
    QString filename=LocalData::instance()->getLocalPath().c_str();

    filename = filename + "/boundaries-v3.db";

    std::vector<BoundaryCondition*> boundaries=readBoundaryConditionsFromFile(filename);
    for (unsigned int i=0 ; i<boundaries.size() ; i++)
    {
        BoundaryCondition *bc=boundaries.at(i);
        BoundaryConditions::instance()->addBcToUserDb(bc);
        addBoundaryConditionToView(bc, _userBCNode);
    }
    _userBCNode.sortChildren(0, Qt::AscendingOrder);
}

std::vector<BoundaryCondition*> BoundaryConditionList::readBoundaryConditionsFromFile(const QString &filename) const
{
    std::vector<BoundaryCondition*> out;

    // Read database content.
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return out;

    QString xml=file.readAll();
    file.close();

    // Get materials.
    TiXmlDocument doc;
    doc.Parse(xml.toStdString().c_str());

    TiXmlElement *boundaries=doc.RootElement();
    if (!boundaries)
        return out;

    TiXmlElement *boundary=boundaries->FirstChildElement("boundary-condition");
    while (boundary)
    {
        std::string id;
        if (boundary->Attribute("id"))
            id=boundary->Attribute("id");

        std::string name;
        if (boundary->FirstChildElement("name") && boundary->FirstChildElement("name")->GetText())
            name=boundary->FirstChildElement("name")->GetText();

        std::string description;
        if (boundary->FirstChildElement("description") && boundary->FirstChildElement("description")->GetText())
            description=boundary->FirstChildElement("description")->GetText();

        std::string temperature;
        if (boundary->FirstChildElement("temperature") && boundary->FirstChildElement("temperature")->GetText())
            temperature=boundary->FirstChildElement("temperature")->GetText();

        std::string resistance;
        if (boundary->FirstChildElement("resistance") && boundary->FirstChildElement("resistance")->GetText())
            resistance=boundary->FirstChildElement("resistance")->GetText();

        std::string rh;
        if (boundary->FirstChildElement("humidity") && boundary->FirstChildElement("humidity")->GetText())
            rh = boundary->FirstChildElement("humidity")->GetText();

        std::string red;
        std::string green;
        std::string blue;
        TiXmlElement *color=boundary->FirstChildElement("color");
        if (color)
        {
            if (color->FirstChildElement("red") && color->FirstChildElement("red")->GetText())
                red=color->FirstChildElement("red")->GetText();
            if (color->FirstChildElement("green") && color->FirstChildElement("green")->GetText())
                green=color->FirstChildElement("green")->GetText();
            if (color->FirstChildElement("blue") && color->FirstChildElement("blue")->GetText())
                blue=color->FirstChildElement("blue")->GetText();
        }
        
        BoundaryCondition bc;
        bc.setId(id);
        bc.setName(name);
        bc.setDescription(description);
        bc.setTemperature(ToolBox::stringToDouble(temperature));
        bc.setResistance(ToolBox::stringToDouble(resistance), 0);
        bc.setColor(ToolBox::stringToInt(red), ToolBox::stringToInt(green), ToolBox::stringToInt(blue));

        if (!rh.empty())
            bc.setRh(ToolBox::stringToDouble(rh), 0);

        if (bc.check(0))
        {
            BoundaryCondition *bc2=new BoundaryCondition(bc);
            out.push_back(bc2);
        }

        boundary=boundary->NextSiblingElement("boundary-condition");
    }
    
    return out;
}

void BoundaryConditionList::addBoundaryConditionToView(BoundaryCondition *bc, QTreeWidgetItem &root)
{
    BoundaryConditionCustomItem *item=new BoundaryConditionCustomItem;
    item->setBoundaryCondition(bc);

    item->setTitle(bc->name().c_str());
    item->setResistance(bc->resistance());
    item->setTemperature(bc->temperature());
    item->setRh(bc->rh());
    item->setColor(QColor(bc->red(), bc->green(), bc->blue()));

    root.addChild(item);
    _list.setItemWidget(item, 0, item);

    if (&root==&_userBCNode)
    {
        connect(item,
                &BoundaryConditionCustomItem::saveDatabaseRequested,
                this,
                &BoundaryConditionList::saveUserDatabase,
                Qt::UniqueConnection);
        item->setEditable(true);
    }
    else
    {
        item->setEditable(false);
    }
}

void BoundaryConditionList::addNewBoundaryCondition(QTreeWidgetItem *item)
{
    CreateBoundaryCondition *action=new CreateBoundaryCondition;
    ActionsManager::instance()->exec(action);

    // Set default data.
    std::string id=action->boundaryConditionId();
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(id);
    if (!bc)
        return;

    bc->setColor(rand()%256, rand()%256, rand()%256);
    bc->setName(LinguistManager::instance()->translate("Nouvelle condition à la limite").toStdString());
}

void BoundaryConditionList::removeBoundaryConditionById(QTreeWidgetItem *item, const std::string &id)
{
    for (int i=0 ; i<item->childCount() ; i++)
    {
        QTreeWidgetItem *t=item->child(i);
        BoundaryConditionCustomItem *bc=dynamic_cast<BoundaryConditionCustomItem*>(t);
        if (!bc)
            continue;

        if (bc->id().toStdString()==id)
        {
            bc->deleteLater();
            return;
        }
    }
}
