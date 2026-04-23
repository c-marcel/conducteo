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

#include <materials/MaterialCategoryEditor.h>
#include <QLineEdit>

MaterialCategoryEditor::MaterialCategoryEditor(QObject *parent):
    QItemDelegate(parent)
{
}

MaterialCategoryEditor::~MaterialCategoryEditor()
{
}

QWidget *MaterialCategoryEditor::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor=new QLineEdit(parent);
    return editor;
}

void MaterialCategoryEditor::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit=(QLineEdit*) editor;
    QString title=index.model()->data(index, Qt::EditRole).toString();
    edit->setText(title);
    edit->setFocus();
}

void MaterialCategoryEditor::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect r(option.rect.left()+2, option.rect.top()+1, option.rect.width()-4, option.rect.height()-2);
    editor->setGeometry(r);
}

void MaterialCategoryEditor::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit=(QLineEdit*) editor;
    QString title=edit->text();
    model->setData(index, title, Qt::EditRole);
    emit closeEditorRequested();
}
