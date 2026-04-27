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

#include <materials/MaterialList.h>
#include <materials/MaterialCategoryEditor.h>
#include <actions/materials/CreateMaterial.h>
#include <actions/materials/DeleteMaterial.h>
#include <actions/materials/CreateCategory.h>
#include <actions/materials/DeleteCategory.h>
#include <actions/materials/RenameCategory.h>
#include <materials/MaterialCustomItem.h>
#include <materials/MaterialCategory.h>
#include <actions/ActionsManager.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <LinguistManager.h>
#include <LinguistManager.h>
#include <tools/ToolBox.h>
#include <tools/UiTools.h>
#include <StatesManager.h>
#include <QApplication>
#include <LocalData.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <tinyxml.h>
#include <QDrag>
#include <QMenu>

MaterialList::MaterialList(QWidget *parent):
    QWidget(parent),
    _layout(this),
    _headers(Qt::Horizontal),
    _editedCategory(0),
    _extrusion(0)
{
    setFixedWidth(290);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Material list.
    _list.setHeader(&_headers);
    _list.setIndentation(10);
    _list.setContextMenuPolicy(Qt::CustomContextMenu);

    QFont f = font();
    f.setPixelSize(12);
    _list.setFont(f);

    _headers.resizeSection(0, 200);
    _headers.setBaseSize(200, 50);

    // Add root nodes.
    addExtrusionMaterial();

    _lastMaterialNode.setFlags(Qt::ItemIsEnabled);
    _lastMaterialNode.setSizeHint(0, QSize(150, 25));

    _airMaterialNode.setFlags(Qt::ItemIsEnabled);
    _airMaterialNode.setSizeHint(0, QSize(150, 25));

    _userMaterialNode.setFlags(Qt::ItemIsEnabled);
    _userMaterialNode.setSizeHint(0, QSize(150, 25));

    _rtMaterialNode.setFlags(Qt::ItemIsEnabled);
    _rtMaterialNode.setSizeHint(0, QSize(150, 25));

    _list.addTopLevelItem(&_lastMaterialNode);
    _list.addTopLevelItem(&_airMaterialNode);
    _list.addTopLevelItem(&_userMaterialNode);
    _list.addTopLevelItem(&_rtMaterialNode);

    _userMaterialNode.setExpanded(true);

    connect(&_list, &QTreeWidget::currentItemChanged, this, &MaterialList::currentItemChanged);
    connect(&_list, &QTreeWidget::itemDoubleClicked, this, &MaterialList::itemDoubleClicked);
    connect(&_list,
            &TreeWidget::customContextMenuRequested,
            this,
            qOverload<const QPoint &>(&MaterialList::customContextMenuRequested));
    connect(&_list,
            &TreeWidget::deleteMaterialRequested,
            this,
            &MaterialList::deleteMaterialRequested);

    _layout.addWidget(&_list);

    MaterialCategoryEditor *editor=new MaterialCategoryEditor(this);
    connect(editor,
            &MaterialCategoryEditor::closeEditorRequested,
            this,
            &MaterialList::closeEditorRequested);
    _list.setItemDelegate(editor);

    translate();
    applyTheme();
    statesChanged();

    // Copy Rt 2012 database if needed.
    QString destination=LocalData::instance()->getLocalPath().c_str();

    destination = destination + "/rt2012-v3.db";

    QString source= UiTools::getDataDir() + "/rt2012.db";
    if(!QFile::exists(destination))
        QFile::copy(source, destination);

    // Copy Rt 2012 if user has old version.
    QString currentVersion  = readRt2012DatabaseVersion(destination);
    QString deployedVersion = readRt2012DatabaseVersion(source);

    if (currentVersion != deployedVersion)
    {
        QFile::remove(destination);
        QFile::copy(source, destination);
    }

    // Load databases.
    loadRtDatabase();
    loadUserDatabase();
    loadLastMaterialsUsed();

    Materials::instance()->registerObserver(this);
    Materials::instance()->loadDefaultMaterials();
}

MaterialList::~MaterialList()
{
}

void MaterialList::loadRtDatabase()
{
    QString filename=LocalData::instance()->getLocalPath().c_str();

    filename = filename + "/rt2012-v3.db";

    std::vector<Material*> materials=readMaterialsFromFile(filename);
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *material=materials.at(i);
        Materials::instance()->addMaterialToRtDb(material);
        addMaterialToView(material, &_rtMaterialNode);
    }
    sortListRequested();
}

void MaterialList::loadUserDatabase()
{
    QString filename=LocalData::instance()->getLocalPath().c_str();

    filename = filename + "/materials-v3.db";

    // Import all categories.
    importUserCategories(filename);

    std::vector<Material*> materials=readMaterialsFromFile(filename);
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *material=materials.at(i);
        Materials::instance()->addMaterialToUserDb(material);

        std::string cat_id=material->category();
        QTreeWidgetItem *category=categoryById(&_userMaterialNode ,cat_id.c_str());
        if (!category)
            category=&_userMaterialNode;

        addMaterialToView(material, category);
    }
    sortListRequested();
}

bool MaterialList::categoryExists(QTreeWidgetItem *root, const std::string &id) const
{
    QString qid=id.c_str();
    for (int i=0 ; i<root->childCount() ; i++)
    {
        MaterialCategory *category=dynamic_cast<MaterialCategory*>(root->child(i));
        if (!category)
            continue;

        if (category->id()==qid)
            return true;

        if (categoryExists(category, id))
            return true;
    }
    return false;
}

void MaterialList::importUserCategories(const QString &filename)
{
    // Read database content.
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QString xml=file.readAll();
    file.close();

    // Get materials.
    TiXmlDocument doc;
    doc.Parse(xml.toStdString().c_str());

    TiXmlElement *materials=doc.RootElement();
    if (!materials)
        return;

    TiXmlElement *categories=materials->FirstChildElement("categories");
    if (!categories)
        return;

    TiXmlElement *category=categories->FirstChildElement("category");
    if (!category)
        return;

    std::vector<MaterialCategory*> items;
    while (category)
    {
        std::string id;
        if (category->Attribute("id"))
            id=category->Attribute("id");

        std::string name;
        if (category->Attribute("name"))
            name=category->Attribute("name");

        std::string parentId;
        if (category->Attribute("parent"))
            parentId=category->Attribute("parent");

        if (!categoryExists(&_userMaterialNode, id))
        {
            MaterialCategory *cat=new MaterialCategory(&_userMaterialNode);
            cat->setId(id.c_str());
            cat->setParentId(parentId.c_str());
            cat->setTitle(name.c_str());
            items.push_back(cat);
        }

        category=category->NextSiblingElement("category");
    }

    // Set parent.
    for (unsigned int i=0 ; i<items.size() ; i++)
    {
        MaterialCategory *category=items.at(i);
        QString parentId=category->parentId();
        if (parentId.isEmpty())
            continue;

        QTreeWidgetItem *p=categoryById(&_userMaterialNode, parentId);
        if (p)
        {
            int idx=_userMaterialNode.indexOfChild(category);
            QTreeWidgetItem *copy=_userMaterialNode.takeChild(idx);
            p->addChild(copy);
        }
    }
}

QTreeWidgetItem *MaterialList::categoryById(QTreeWidgetItem *root, const QString &id) const
{
    for (int i=0 ; i<root->childCount() ; i++)
    {
        MaterialCategory *category=dynamic_cast<MaterialCategory*>(root->child(i));
        if (!category)
            continue;

        if (category->id()==id)
            return category;

        QTreeWidgetItem *item=categoryById(category, id);
        if (item)
            return item;
    }
    return 0;
}

void MaterialList::addMaterialToView(Material *material, QTreeWidgetItem *root)
{
    MaterialCustomItem *item=new MaterialCustomItem;
    item->setMaterial(material);

    item->setTitle(material->name().c_str());
    item->setConductivity(material->conductivity());
    item->setColor(QColor(material->red(), material->green(), material->blue()));
    item->setEmissivity(material->emissivity());
    item->setMu(material->mu());
    item->setDescription(material->description().c_str());
    item->setFavorite(material->favorite());

    root->addChild(item);
    _list.setItemWidget(item, 0, item);

    connect(item,
            &MaterialCustomItem::saveDatabaseRequested,
            this,
            &MaterialList::saveUserDatabase,
            Qt::UniqueConnection);
    connect(item,
            &MaterialCustomItem::sortListRequested,
            this,
            &MaterialList::sortListRequested,
            Qt::UniqueConnection);
    connect(&_headers,
            &SearchInput::searchInputTextChanged,
            item,
            &MaterialCustomItem::applyFilter,
            Qt::UniqueConnection);
}

MaterialCustomItem *MaterialList::materialItemFromMaterial(QTreeWidgetItem *root_node, const QString &id) const
{
    for (int i=0 ; i<root_node->childCount() ; i++)
    {
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(root_node->child(i));
        if (material && material->id() == id)
            return material;
        MaterialCategory *category=dynamic_cast<MaterialCategory*>(root_node->child(i));
        if (category)
        {
            MaterialCustomItem *m = materialItemFromMaterial(category, id);
            if (m)
                return m;
        }
    }

    return 0;
}

void MaterialList::setSelected(QTreeWidgetItem *root_node, const QString &id, bool selected)
{
    for (int i=0 ; i<root_node->childCount() ; i++)
    {
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(root_node->child(i));
        if (material && material->id() == id)
            material->setSelected(selected);
        else if (material)
            material->setSelected(false);

        MaterialCategory *category=dynamic_cast<MaterialCategory*>(root_node->child(i));
        if (category)
            setSelected(category, id, selected);
    }
}

void MaterialList::addExtrusionMaterial()
{
    if (_extrusion)
        return;

    _extrusion = new MaterialCustomItem;

    _extrusion->setTitle(_tr("Extruder la modélisation"));
    _extrusion->setExtrusion(true);

    _list.addTopLevelItem(_extrusion);
    _list.setItemWidget(_extrusion, 0, _extrusion);
}

std::vector<Material*> MaterialList::readMaterialsFromFile(const QString &filename) const
{
    std::vector<Material*> out;

    // Read database content.
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return out;
    QString xml=file.readAll();
    file.close();

    // Get materials.
    TiXmlDocument doc;
    doc.Parse(xml.toStdString().c_str());

    TiXmlElement *materials=doc.RootElement();
    if (!materials)
        return out;

    TiXmlElement *material=materials->FirstChildElement("material");
    while (material)
    {
        std::string id;
        if (material->Attribute("id"))
            id=material->Attribute("id");

        std::string name;
        if (material->FirstChildElement("name") && material->FirstChildElement("name")->GetText())
            name=material->FirstChildElement("name")->GetText();

        std::string description;
        if (material->FirstChildElement("description") && material->FirstChildElement("description")->GetText())
            description=material->FirstChildElement("description")->GetText();

        std::string conductivity;
        if (material->FirstChildElement("conductivity") && material->FirstChildElement("conductivity")->GetText())
            conductivity=material->FirstChildElement("conductivity")->GetText();

        std::string emissivity;
        if (material->FirstChildElement("emissivity") && material->FirstChildElement("emissivity")->GetText())
            emissivity=material->FirstChildElement("emissivity")->GetText();

        std::string mu;
        if (material->FirstChildElement("mu") && material->FirstChildElement("mu")->GetText())
            mu=material->FirstChildElement("mu")->GetText();

        std::string favorite;
        if (material->FirstChildElement("favorite") && material->FirstChildElement("favorite")->GetText())
            favorite=material->FirstChildElement("favorite")->GetText();

        std::string category;
        if (material->FirstChildElement("category") && material->FirstChildElement("category")->GetText())
            category=material->FirstChildElement("category")->GetText();

        std::string red;
        std::string green;
        std::string blue;
        TiXmlElement *color=material->FirstChildElement("color");
        if (color)
        {
            if (color->FirstChildElement("red") && color->FirstChildElement("red")->GetText())
                red=color->FirstChildElement("red")->GetText();
            if (color->FirstChildElement("green") && color->FirstChildElement("green")->GetText())
                green=color->FirstChildElement("green")->GetText();
            if (color->FirstChildElement("blue") && color->FirstChildElement("blue")->GetText())
                blue=color->FirstChildElement("blue")->GetText();
        }
        
        Material m;
        m.setId(id);
        m.setName(name);
        m.setDescription(description);
        m.setConductivity(ToolBox::stringToDouble(conductivity), 0);
        if (emissivity.empty())
            m.setEmissivity(0.9, 0);
        else
            m.setEmissivity(ToolBox::stringToDouble(emissivity), 0);

        if (mu.empty())
            m.setMu(1.0, 0);
        else
            m.setMu(ToolBox::stringToDouble(mu), 0);

        m.setFavorite(favorite=="true" ? true : false);
        m.setColor(ToolBox::stringToInt(red), ToolBox::stringToInt(green), ToolBox::stringToInt(blue));
        m.setCategory(category);

        if (m.check(0))
        {
            Material *m2=new Material(m);
            out.push_back(m2);
        }

        material=material->NextSiblingElement("material");
    }
    
    return out;
}

void MaterialList::customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item=_list.itemAt(pos);
    
    // Add menu.
    if (item==&_userMaterialNode)
    {
        QMenu menu(this);
        QPoint globalPos=_list.mapToGlobal(pos);
        globalPos.setY(globalPos.y()+_headers.height());
        QAction *add=menu.addAction(_tr("Ajouter un matériau"));
        QAction *addCategory=menu.addAction(_tr("Ajouter une catégorie"));
        menu.addSeparator();
        QAction *exportMaterialDb=menu.addAction(_tr("Exporter la bibliothèque personnelle"));
        QAction *importMaterialDb=menu.addAction(_tr("Importer des matériaux"));
        QAction *choice=menu.exec(globalPos);
        if (choice==add)
            addNewMaterial(item);
        else if (choice==exportMaterialDb)
            exportUserMaterialDatabase();
        else if (choice==importMaterialDb)
            importMaterials();
        else if (choice==addCategory)
        {
            CreateCategory *action=new CreateCategory("");
            ActionsManager::instance()->exec(action);
        }
    }

    // Remove/modify menu.
    else if (item)
    {
        // Click on material.
        MaterialCustomItem *materialItem=dynamic_cast<MaterialCustomItem*>(item);
        if (materialItem)
        {

            QString id=materialItem->id();
            bool editable=true;
            if (id=="106D6720-211C-4090-8C60-7CED671C8580")
                editable=false;
            if (id=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
                editable=false;

            if (editable)
            {
                QMenu menu(this);
                QPoint globalPos=_list.mapToGlobal(pos);
                globalPos.setY(globalPos.y()+_headers.height());
                QAction *remove=menu.addAction(_tr("Supprimer le matériau"));
                QAction *modify=menu.addAction(_tr("Modifier le matériau"));
                QAction *choice=menu.exec(globalPos);
                if (choice==remove)
                {
                    MaterialCustomItem *m=dynamic_cast<MaterialCustomItem*>(item);
                    if (!m)
                        return;

                    QString id=m->id();

                    Material *material=Materials::instance()->materialFromId(id.toStdString());
                    if (!material)
                        return;

                    DeleteMaterial *action=new DeleteMaterial(material);
                    ActionsManager::instance()->exec(action);
                }
                else if (choice==modify)
                {
                    itemDoubleClicked(item, 0);
                }
            }
        }

        // Click on category.
        MaterialCategory *categoryItem=dynamic_cast<MaterialCategory*>(item);
        if (categoryItem)
        {
            QMenu menu(this);
            QPoint globalPos=_list.mapToGlobal(pos);
            globalPos.setY(globalPos.y()+_headers.height());
            QAction *add_material=menu.addAction(_tr("Ajouter un matériau"));
            menu.addSeparator();
            QAction *add=menu.addAction(_tr("Ajouter une catégorie"));
            QAction *remove=menu.addAction(_tr("Supprimer la catégorie"));
            QAction *modify=menu.addAction(_tr("Renommer la catégorie"));
            QAction *choice=menu.exec(globalPos);

            if (choice==add)
            {
                CreateCategory *action=new CreateCategory(categoryItem->id().toStdString());
                ActionsManager::instance()->exec(action);
            }

            else if (choice==add_material)
            {
                addNewMaterial(categoryItem);
            }

            else if (choice==remove)
            {
                DeleteCategory *action=createDeleteCategory(categoryItem);
                ActionsManager::instance()->exec(action);
            }

            else if (choice==modify)
            {
                _editedCategory=categoryItem;
                _list.openPersistentEditor(_editedCategory);
            }
        }
    }
}

std::vector<DeleteMaterial*> MaterialList::childMaterials(MaterialCategory *category) const
{
    std::vector<DeleteMaterial*> child_materials;
    for (int m=0 ; m<category->childCount() ; m++)
    {
        QTreeWidgetItem *t=category->child(m);
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(t);
        if (!material)
            continue;

        std::string id=material->id().toStdString();
        Material *mat=Materials::instance()->materialFromId(id);
        if (!mat)
            continue;

        DeleteMaterial *action=new DeleteMaterial(mat);
        child_materials.push_back(action);
    }
    return child_materials;
}

std::vector<DeleteCategory*> MaterialList::childCategories(MaterialCategory *category) const
{
    std::vector<DeleteCategory*> child_categories;
    for (int m=0 ; m<category->childCount() ; m++)
    {
        QTreeWidgetItem *t=category->child(m);
        MaterialCategory *cat=dynamic_cast<MaterialCategory*>(t);
        if (!cat)
            continue;

        DeleteCategory *action=createDeleteCategory(cat);
        child_categories.push_back(action);
    }
    return child_categories;
}

DeleteCategory *MaterialList::createDeleteCategory(MaterialCategory *category) const
{
    // Create materials list.
    std::vector<DeleteMaterial*> materials=childMaterials(category);

    // Create caterogies list.
    std::vector<DeleteCategory*> categories=childCategories(category);

    MaterialCategory *parent=dynamic_cast<MaterialCategory*>(category->parent());
    std::string parentId;
    if (parent)
        parentId=parent->id().toStdString();

    DeleteCategory *action=new DeleteCategory(category->id().toStdString(), parentId, category->title().toStdString(), categories, materials);
    return action;
}

void MaterialList::translate()
{
    _headers.setTitle(_tr("Matériaux"));
    _lastMaterialNode.setText(0, _tr("Derniers utilisés"));
    _userMaterialNode.setText(0, _tr("Bibliothèque personnelle"));
    _rtMaterialNode.setText(0, _tr("Bibliothèque RE 2020"));
    _airMaterialNode.setText(0, _tr("Cavités d'air"));
}

void MaterialList::applyTheme()
{
    _headers.setIcon(QPixmap(":/images/orange-blue/material.png"));

    QColor themedColor = QColor(199, 229, 245);

    _headers.setBackgroundColor(themedColor);
    update();
}

void MaterialList::statesChanged()
{
    StatesManager::InterfaceMode mode=StatesManager::instance()->interfaceMode();
    StatesManager::ViewType type=StatesManager::instance()->viewType();

    if ((mode==StatesManager::Selection && (type==StatesManager::DisplayModel || type==StatesManager::Display1DModel)) || mode==StatesManager::AddRectangle || mode==StatesManager::AddPolyline || mode==StatesManager::AddCircle || mode==StatesManager::AddEllipse || mode==StatesManager::SetPhysics)
        show();
    else
        hide();
}

void MaterialList::materialDeleted(const std::string &id)
{
    removeMaterialById(&_lastMaterialNode, id);
    removeMaterialById(&_userMaterialNode, id);
    saveUserDatabase();
}

void MaterialList::removeMaterialById(QTreeWidgetItem *item, const std::string &id)
{
    for (int i=0 ; i<item->childCount() ; i++)
    {
        QTreeWidgetItem *t=item->child(i);
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(t);
        if (material && material->id().toStdString()==id)
        {
            material->deleteLater();
            return;
        }

        MaterialCategory *category=dynamic_cast<MaterialCategory*>(t);
        if (category)
            removeMaterialById(category, id);
    }
}

void MaterialList::materialAdded(const std::string &id)
{
    Material *material=Materials::instance()->materialFromId(id);
    if (!material)
        return;

    QTreeWidgetItem *root=categoryById(&_userMaterialNode, material->category().c_str());
    if (!root)
        root=&_userMaterialNode;

    // Default materials.
    if (id=="106D6720-211C-4090-8C60-7CED671C8580" || id=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
        root=&_airMaterialNode;

    addMaterialToView(material, root);
    sortListRequested();
    saveUserDatabase();
}

void MaterialList::materialModified(const std::string &id)
{
    saveUserDatabase();
}

void MaterialList::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    MaterialCustomItem *item=dynamic_cast<MaterialCustomItem*>(current);
    if (item)
        StatesManager::instance()->setCurrentMaterialId(item->id().toStdString());
    else
        StatesManager::instance()->setCurrentMaterialId("");
}

void MaterialList::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(item);
    if (!material)
        return;

    expandChildren(&_lastMaterialNode, item);
    expandUserChildren(&_userMaterialNode, item);
    expandChildren(&_rtMaterialNode, item);
    _list.resizeColumnToContents(0);
}

void MaterialList::expandChildren(QTreeWidgetItem *root_node, QTreeWidgetItem *item)
{
    for (int i=0 ; i<root_node->childCount() ; i++)
    {
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(root_node->child(i));
        if (!material)
            continue;
        material->expandItem(item);
    }
}

void MaterialList::expandUserChildren(QTreeWidgetItem *root_node, QTreeWidgetItem *item)
{
    for (int i=0 ; i<root_node->childCount() ; i++)
    {
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(root_node->child(i));
        if (material)
            material->expandItem(item);
        MaterialCategory *category=dynamic_cast<MaterialCategory*>(root_node->child(i));
        if (category)
            expandUserChildren(category, item);
    }
}

void MaterialList::exportUserMaterialDatabase()
{
    QFileDialog fileDialog(this, _tr("Exporter des matériaux"));
    fileDialog.setNameFilter(_tr("Matériaux conducteö (*.db)"));
    fileDialog.setDefaultSuffix("db");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    if(!fileDialog.exec())
        return;

    QString f=fileDialog.selectedFiles().first();
    if (f.isEmpty())
        return;

    QString filename=LocalData::instance()->getLocalPath().c_str();

    filename = filename + "/materials-v3.db";

    QFile::copy(filename, f);
}

void MaterialList::importMaterials()
{
    QFileDialog fileDialog(this, _tr("Importer des matériaux"));
    fileDialog.setNameFilter(_tr("Matériaux conducteö (*.db)"));
    fileDialog.setDefaultSuffix("db");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    if(!fileDialog.exec())
        return;

    QString f=fileDialog.selectedFiles().first();
    if (f.isEmpty())
        return;

    importMaterialsDatabase(f);
}

void MaterialList::importMaterialsDatabase(const QString &filename)
{
    // Import categories.
    importUserCategories(filename);

    std::vector<Material*> materials=readMaterialsFromFile(filename);

    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *material=materials.at(i);

        // Delete material if already defined.
        Material *m=Materials::instance()->materialFromId(material->id());
        if (!m)
            m=Materials::instance()->duplicate(material);
        if (m)
            delete material;
        else
        {
            Materials::instance()->addMaterialToUserDb(material);
        }
    }
    sortListRequested();
    saveUserDatabase();
}

QString MaterialList::categoriesSerializer(QTreeWidgetItem *root) const
{
    QString xml;
    for (int i=0 ; i<root->childCount() ; i++)
    {
        MaterialCategory *category=dynamic_cast<MaterialCategory*>(root->child(i));
        if (!category)
            continue;

        xml+=category->serialize().c_str();
        xml+=categoriesSerializer(category);
    }
    return xml;
}

std::vector<MaterialCategory *> MaterialList::categories() const
{
    std::vector<MaterialCategory *> categories;
    for (int i=0 ; i<_userMaterialNode.childCount() ; i++)
    {
        MaterialCategory *category=dynamic_cast<MaterialCategory*>(_userMaterialNode.child(i));
        if (!category)
            continue;
        categories.push_back(category);
    }
    return categories;
}

void MaterialList::selectMaterialById(const QString &id)
{
    _extrusion->setSelected(false);
    setSelected(&_userMaterialNode, id, true);
    setSelected(&_airMaterialNode, id, true);
    setSelected(&_rtMaterialNode, id, true);

    MaterialCustomItem *item = materialItemFromMaterial(&_userMaterialNode, id);
    if (item)
        _list.setCurrentItem(item);

    item = materialItemFromMaterial(&_airMaterialNode, id);
    if (item)
        _list.setCurrentItem(item);

    item = materialItemFromMaterial(&_rtMaterialNode, id);
    if (item)
        _list.setCurrentItem(item);
}

QString MaterialList::userMaterialSerializer(QTreeWidgetItem *root) const
{
    QString xml;
    for (int i=0 ; i<root->childCount() ; i++)
    {
        QTreeWidgetItem *item=root->child(i);
        MaterialCustomItem *material=dynamic_cast<MaterialCustomItem*>(item);
        if (material)
        {
            QString id=material->id();

            if (id!="106D6720-211C-4090-8C60-7CED671C8580" && id!="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
            {
                Material *m=Materials::instance()->materialFromId(id.toStdString());
                if (!m)
                    continue;
                xml+=m->serialize().c_str();
            }
        }

        MaterialCategory *category=dynamic_cast<MaterialCategory*>(item);
        if (category)
            xml+=userMaterialSerializer(category);
    }
    return xml;
}

void MaterialList::saveUserDatabase()
{
    QByteArray xml="<materials>";

    xml.append("<categories>");
    xml.append(categoriesSerializer(&_userMaterialNode).toUtf8());
    xml.append("</categories>");
    xml.append(userMaterialSerializer(&_userMaterialNode).toUtf8());
    xml.append("</materials>");
    
    QString filename=LocalData::instance()->getLocalPath().c_str();

    filename = filename + "/materials-v3.db";

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(xml);
    file.close();
}

void MaterialList::addNewMaterial(QTreeWidgetItem *item)
{
    std::string category_id;
    MaterialCategory *category=dynamic_cast<MaterialCategory*>(item);
    if (category)
        category_id=category->id().toStdString();

    CreateMaterial *action=new CreateMaterial(category_id);
    ActionsManager::instance()->exec(action);

    // Set default data.
    std::string id=action->materialId();
    Material *material=Materials::instance()->materialFromId(id);
    if (!material)
        return;

    material->setColor(rand()%256, rand()%256, rand()%256);
    material->setName(LinguistManager::instance()->translate("Nouveau matériau").toStdString());

    Materials::instance()->materialModified(id);

    sortListRequested();
    saveUserDatabase();

    // Expand new material.
    MaterialCustomItem *m = materialItemFromMaterial(&_userMaterialNode, material->id().c_str());
    if (m)
    {
        expandChildren(&_lastMaterialNode, m);
        expandUserChildren(&_userMaterialNode, m);
        expandChildren(&_rtMaterialNode, m);
        _list.resizeColumnToContents(0);

        m->setSelected(true);
    }
}

void MaterialList::loadLastMaterialsUsed()
{
    std::string last=LocalData::instance()->getParameter("lastMaterials");
    std::vector<std::string> materials=ToolBox::split(last, ';');
    materials=ToolBox::invert(materials);

    // Save current selected item if present into last used materials list.
    QString current_id;
    QTreeWidgetItem *current=_list.currentItem();
    MaterialCustomItem *material_item=dynamic_cast<MaterialCustomItem*>(current);
    if (material_item && current->parent()==&_lastMaterialNode)
        current_id=material_item->id();

    // Clear list.
    while(_lastMaterialNode.childCount()>0)
    {
        QTreeWidgetItem *item=_lastMaterialNode.child(0);
        _lastMaterialNode.removeChild(item);
    }

    // Add materials.
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *material=Materials::instance()->materialFromId(materials.at(i));
        if (!material)
            continue;

        addMaterialToView(material, &_lastMaterialNode);
    }

    // Set selected material.
    if (!current_id.isEmpty())
    {
        for (int i=0 ; i<_lastMaterialNode.childCount() ; i++)
        {
            QTreeWidgetItem *item=_lastMaterialNode.child(i);
            MaterialCustomItem *material_item=dynamic_cast<MaterialCustomItem*>(item);
            if (material_item && material_item->id()==current_id)
                _list.setCurrentItem(item);
        }
    }
}

void MaterialList::setMaterialUsed(const QString &id)
{
    std::string s_id=id.toStdString();

    // Read only materials.
    if (s_id=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
        return;
    if (s_id=="106D6720-211C-4090-8C60-7CED671C8580")
        return;

    std::string last=LocalData::instance()->getParameter("lastMaterials");
    std::vector<std::string> materials=ToolBox::split(last, ';');
    std::vector<std::string> materials2;
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        if (materials.at(i)!=s_id)
            materials2.push_back(materials.at(i));
    }
    materials2.push_back(s_id);
    LocalData::instance()->setParameter("lastMaterials", ToolBox::merge(materials2, ';'));
    loadLastMaterialsUsed();
}

void MaterialList::sortListRequested()
{
    _userMaterialNode.sortChildren(0, Qt::AscendingOrder);
    _rtMaterialNode.sortChildren(0, Qt::AscendingOrder);
    sortRecurse(&_userMaterialNode);
}

void MaterialList::sortRecurse(QTreeWidgetItem *category)
{
    category->sortChildren(0, Qt::AscendingOrder);
    for (int i=0 ; i<category->childCount() ; i++)
    {
        QTreeWidgetItem *item=category->child(i);
        MaterialCategory *cat=dynamic_cast<MaterialCategory*>(item);
        if (!cat)
            continue;
        sortRecurse(cat);
    }
}

QString MaterialList::readRt2012DatabaseVersion(const QString &filename) const
{
    // Read database content.
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return "";
    QString xml = file.readAll();
    file.close();

    // Get materials.
    TiXmlDocument doc;
    doc.Parse(xml.toStdString().c_str());

    TiXmlElement *materials = doc.RootElement();
    if (!materials)
        return "1";

    if (!materials->Attribute("version"))
        return "1";

    return materials->Attribute("version");
}

void MaterialList::closeEditorRequested()
{
    if (!_editedCategory)
        return;
    _list.closePersistentEditor(_editedCategory);

    std::string old_title=_editedCategory->title().toStdString();
    std::string new_title=_editedCategory->data(0, Qt::EditRole).toString().toStdString();
    if (old_title!=new_title)
    {
        RenameCategory *action=new RenameCategory(_editedCategory->id().toStdString(), old_title, new_title);
        ActionsManager::instance()->exec(action);
    }
    _editedCategory=0;
}

void MaterialList::categoryDeleted(const std::string &id)
{
    // Move all materials into parent category.
    MaterialCategory *category=dynamic_cast<MaterialCategory*>(categoryById(&_userMaterialNode, id.c_str()));
    if (!category)
        return;

    QTreeWidgetItem *parent=category->parent();
    if (!parent)
        return;

    // Delete category.
    parent->removeChild(category);

    sortListRequested();
    saveUserDatabase();
}

void MaterialList::categoryAdded(const std::string &id, const std::string &parentId, const std::string &name)
{
    QTreeWidgetItem *parent=categoryById(&_userMaterialNode, parentId.c_str());
    if (!parent)
        parent=&_userMaterialNode;
    MaterialCategory *new_cat=new MaterialCategory(parent);
    new_cat->setId(id.c_str());
    QString title=name.c_str();
    if (title.isEmpty())
        title=_tr("Nouvelle catégorie");
    new_cat->setTitle(title);
    sortListRequested();
    saveUserDatabase();
}

void MaterialList::categoryRenamed(const std::string &id, const std::string &old_name, const std::string &new_name)
{
    MaterialCategory *category=dynamic_cast<MaterialCategory*>(categoryById(&_userMaterialNode, id.c_str()));
    if (!category)
        return;
    category->setTitle(new_name.c_str());
    sortListRequested();
    saveUserDatabase();
}

void MaterialList::deleteMaterialRequested(const QString &id)
{
    Material *material=Materials::instance()->materialFromId(id.toStdString());
    if (!material)
        return;

    DeleteMaterial *action=new DeleteMaterial(material);
    ActionsManager::instance()->exec(action);
}
