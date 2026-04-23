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

#ifndef _MATERIAL_LIST_H
#define _MATERIAL_LIST_H

#include <databases/MaterialsObserver.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <SearchInput.h>
#include <TreeWidget.h>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

class CustomMaterialItemView;
class MaterialCustomItem;
class MaterialCategory;
class DeleteMaterial;
class DeleteCategory;
class Material;

class MaterialList: public QWidget,
                    public Translatable,
                    public StatesObserver,
                    public MaterialsObserver
{
    Q_OBJECT
public:
    MaterialList(QWidget *parent=0);
    virtual ~MaterialList();

    void importMaterialsDatabase(const QString &filename);

    void translate();
    void applyTheme();
    void statesChanged();
    
    void materialDeleted(const std::string &id);
    void materialAdded(const std::string &id);
    void materialModified(const std::string &id);

    void categoryDeleted(const std::string &id);
    void categoryAdded(const std::string &id, const std::string &parentId, const std::string &name);
    void categoryRenamed(const std::string &id, const std::string &old_name, const std::string &new_name);

    std::vector<MaterialCategory *> categories() const;

public slots:
    void setMaterialUsed(const QString &id);
    void selectMaterialById(const QString &id);

private slots:
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void customContextMenuRequested(const QPoint &pos);
    void deleteMaterialRequested(const QString &id);
    void saveUserDatabase();
    void sortListRequested();
    void closeEditorRequested();

private:
    void addExtrusionMaterial();
    void loadRtDatabase();
    void loadUserDatabase();
    void loadLastMaterialsUsed();
    std::vector<Material*> readMaterialsFromFile(const QString &filename) const;
    void addMaterialToView(Material *material, QTreeWidgetItem *root);
    MaterialCustomItem *materialItemFromMaterial(QTreeWidgetItem *root_node, const QString &id) const;
    void setSelected(QTreeWidgetItem *root_node, const QString &id, bool selected);

    void expandChildren(QTreeWidgetItem *root_node, QTreeWidgetItem *item);
    void expandUserChildren(QTreeWidgetItem *root_node, QTreeWidgetItem *item);
    void exportUserMaterialDatabase();
    void importMaterials();
    void addNewMaterial(QTreeWidgetItem *item);
    void removeMaterialById(QTreeWidgetItem *item, const std::string &id);

    void importUserCategories(const QString &filename);
    QTreeWidgetItem *categoryById(QTreeWidgetItem *root, const QString &id) const;
    QString categoriesSerializer(QTreeWidgetItem *root) const;
    QString userMaterialSerializer(QTreeWidgetItem *root) const;
    bool categoryExists(QTreeWidgetItem *root, const std::string &id) const;

    std::vector<DeleteMaterial*> childMaterials(MaterialCategory *category) const;
    std::vector<DeleteCategory*> childCategories(MaterialCategory *category) const;
    DeleteCategory *createDeleteCategory(MaterialCategory *category) const;
    void sortRecurse(QTreeWidgetItem *category);

    QString readRt2012DatabaseVersion(const QString &filename) const;

    QVBoxLayout         _layout;
    TreeWidget          _list;
    SearchInput         _headers;
    QTreeWidgetItem     _lastMaterialNode;
    QTreeWidgetItem     _airMaterialNode;
    QTreeWidgetItem     _userMaterialNode;
    QTreeWidgetItem     _rtMaterialNode;
    MaterialCategory   *_editedCategory;
    MaterialCustomItem *_extrusion;
};

#endif
