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

#include <ComboBoxMaterialDelegate.h>
#include <actions/model/SetVolumeMaterial.h>
#include <actions/ActionsManager.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <QAbstractItemView>
#include <StatesManager.h>
#include <model/Volume.h>
#include <QHelpEvent>
#include <QComboBox>
#include <QToolTip>
#include <QPainter>
#include <QColor>
#include <QEvent>

ComboBoxMaterialDelegate::ComboBoxMaterialDelegate(QComboBox *cb, QObject *parent):
    QAbstractItemDelegate(parent),
    _combobox(cb),
    _catchUserInputs(true),
    _changeVolumeMaterial(true)
{
    Materials::instance()->registerObserver(this);
    connect(cb,
            &QComboBox::currentIndexChanged,
            this,
            &ComboBoxMaterialDelegate::currentIndexChanged);
}

ComboBoxMaterialDelegate::~ComboBoxMaterialDelegate()
{
    Materials::instance()->unregisterObserver(this);
}

QSize ComboBoxMaterialDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(100, 20);
}

void ComboBoxMaterialDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Read material data.
    QString id=index.data(Qt::UserRole).toString();
    Material *material=Materials::instance()->materialFromId(id.toStdString());
    if (!material)
        return;

    double conductivity=material->conductivity();
    QString name=material->name().c_str();

    int r=material->red();
    int g=material->green();
    int b=material->blue();
    QColor color(r, g, b);

    // Selected index.
    painter->setPen(Qt::white);
    painter->setBrush(Qt::white);
    if(option.state & QStyle::State_MouseOver)
        painter->setBrush(QColor(233, 243, 233));
    if(_combobox->currentIndex()==index.row())
        painter->setBrush(QColor(161,192,87));
    painter->drawRect(option.rect);
    painter->setPen(Qt::black);

    // Draw rectangle.
    QBrush brush(color);
    painter->setBrush(brush);
    QRect rect(option.rect.x()+4, option.rect.y()+4, 20, 12);
    painter->drawRect(rect);
    
    // Draw text.
    QLocale locale;
    QString text=name;
    text+=" – λ=";
    text+=locale.toString(conductivity);
    text+=" W/(m.°C)";
    painter->drawText(rect.x()+rect.width()+10, option.rect.y()+3, option.rect.width()-rect.x()-10, 14, Qt::AlignVCenter, text);
}

bool ComboBoxMaterialDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // Display tooltip.
    if (event->type()==QEvent::ToolTip)
    {
        QString id=index.data(Qt::UserRole).toString();
        Material *material=Materials::instance()->materialFromId(id.toStdString());
        if (!material)
            return false;

        double conductivity=material->conductivity();
        QString name=material->name().c_str();

        QLocale locale;
        QString text=name;
        text+=" – λ=";
        text+=locale.toString(conductivity);
        text+=" W/(m.°C)";
        QToolTip::showText(view->mapToGlobal(event->pos()), text);

        return true;
    }
    return false;
}

void ComboBoxMaterialDelegate::materialDeleted(const std::string &s_id)
{
    _catchUserInputs=false;
    QString id=s_id.c_str();
    for (int i=0 ; i<_combobox->count() ; i++)
    {
        QString id2=_combobox->itemData(i, Qt::UserRole).toString();
        if (id==id2)
        {
            if (_combobox->currentIndex()==i)
                _combobox->setCurrentIndex(-1);
            _combobox->removeItem(i);
            _combobox->model()->sort(0, Qt::AscendingOrder);
            break;
        }
    }
    _catchUserInputs=true;
}

void ComboBoxMaterialDelegate::materialModified(const std::string &s_id)
{
    Material *material=Materials::instance()->materialFromId(s_id);
    if (!material)
        return;

    QString id=s_id.c_str();
    for (int i=0 ; i<_combobox->count() ; i++)
    {
        QString id2=_combobox->itemData(i, Qt::UserRole).toString();
        if (id==id2)
        {
            QString name=material->name().c_str();
            double conductivity=material->conductivity();

            QLocale locale;
            QString text=name;
            text+=" – λ=";
            text+=locale.toString(conductivity);
            text+=" W/(m.°C)";
            _combobox->setItemText(i, text);
            _combobox->model()->sort(0, Qt::AscendingOrder);
            break;
        }
    }
}

void ComboBoxMaterialDelegate::materialAdded(const std::string &id)
{
    _catchUserInputs=false;

    Material *material=Materials::instance()->materialFromId(id);
    if (material)
    {
        QString name=material->name().c_str();
        double conductivity=material->conductivity();

        QLocale locale;
        QString text=name;
        text+=" – λ=";
        text+=locale.toString(conductivity);
        text+=" W/(m.°C)";
        _combobox->addItem(text, id.c_str());
        _combobox->model()->sort(0, Qt::AscendingOrder);
    }

    _catchUserInputs=true;
}

void ComboBoxMaterialDelegate::categoryDeleted(const std::string &id)
{
    // Do nothing.
}

void ComboBoxMaterialDelegate::categoryAdded(const std::string &id, const std::string &parentId, const std::string &name)
{
    // Do nothing.
}

void ComboBoxMaterialDelegate::categoryRenamed(const std::string &id, const std::string &old_name, const std::string &new_name)
{
    // Do nothing.
}

void ComboBoxMaterialDelegate::statesChanged()
{
    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.size()!=1)
        return;

    Volume *volume=volumes.at(0);
    Material *material=volume->material();

    _catchUserInputs=false;
    if (!material)
        _combobox->setCurrentIndex(-1);
    else
    {
        QString id=material->id().c_str();
        for (int i=0 ; i<_combobox->count() ; i++)
        {
            QString id2=_combobox->itemData(i, Qt::UserRole).toString();
            if (id==id2)
            {
                _combobox->setCurrentIndex(i);
                break;
            }
        }
    }
    _catchUserInputs=true;
}

void ComboBoxMaterialDelegate::changeVolumeMaterial(bool change)
{
    _changeVolumeMaterial=change;
}

void ComboBoxMaterialDelegate::currentIndexChanged(int idx)
{
    if (!_catchUserInputs)
        return;

    if (!_changeVolumeMaterial)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.size()!=1)
        return;

    Volume *volume=volumes.at(0);
    QString id=_combobox->itemData(idx, Qt::UserRole).toString();

    SetVolumeMaterial *action=new SetVolumeMaterial(model, id.toStdString(), volume);
    ActionsManager::instance()->exec(action);
}

void ComboBoxMaterialDelegate::reloadMaterials()
{
    if (!_combobox)
        return;

    while (_combobox->count()>0)
        _combobox->removeItem(0);

    std::vector<std::string> id=Materials::instance()->materialIdList();
    for (unsigned int i=0 ; i<id.size() ; i++)
        materialAdded(id.at(i));
}
