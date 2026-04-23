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

#ifndef _COMBOBOX_MATERIAL_DELEGATE_H
#define _COMBOBOX_MATERIAL_DELEGATE_H

#include <databases/MaterialsObserver.h>
#include <QAbstractItemDelegate>
#include <StatesObserver.h>

class QComboBox;

class ComboBoxMaterialDelegate: public QAbstractItemDelegate,
                                public MaterialsObserver,
                                public StatesObserver
{

    Q_OBJECT

public:

    ComboBoxMaterialDelegate(QComboBox *cb, QObject *parent=0);
    virtual ~ComboBoxMaterialDelegate();

    void changeVolumeMaterial(bool change);

    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

    virtual bool helpEvent(QHelpEvent * event, QAbstractItemView * view, const QStyleOptionViewItem & option, const QModelIndex & index);

    void reloadMaterials();

    void materialDeleted(const std::string &id);
    void materialAdded(const std::string &id);
    void materialModified(const std::string &id);

    void categoryDeleted(const std::string &id);
    void categoryAdded(const std::string &id, const std::string &parentId, const std::string &name);
    void categoryRenamed(const std::string &id, const std::string &old_name, const std::string &new_name);
    
    void statesChanged();

public slots:

    void currentIndexChanged(int idx);

private:

    QComboBox  *_combobox;
    bool        _catchUserInputs;
    bool        _changeVolumeMaterial;

};

#endif
