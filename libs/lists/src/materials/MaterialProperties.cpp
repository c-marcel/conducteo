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

#include <materials/MaterialProperties.h>
#include <actions/materials/SetMaterialConductivity.h>
#include <actions/materials/SetMaterialDescription.h>
#include <actions/materials/SetMaterialEmissivity.h>
#include <actions/materials/SetMaterialColor.h>
#include <actions/materials/SetMaterialName.h>
#include <actions/materials/SetMaterialMu.h>
#include <actions/ActionsManager.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <StatesManager.h>
#include <QColorDialog>
#include <QHeaderView>
#include <QLocale>

#define ROW_HEIGHT 12

#ifdef _WIN64
#define LIST_HEIGHT 136
#endif

#ifdef __APPLE__
#define LIST_HEIGHT 75
#endif

#ifdef __linux__
#define LIST_HEIGHT 80
#endif

MaterialProperties::MaterialProperties(QWidget *parent):
    QTableWidget(parent),
    _catchUserInputs(true),
    _material(0)
{
    setRowCount(6);
    setColumnCount(2);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);

    QFont f = font();
    f.setPixelSize(10);
    setFont(f);
    
    QTableWidgetItem *item_0=new QTableWidgetItem(_tr("Name"));
    item_0->setFlags(Qt::ItemIsEnabled);
    setItem(0, 0, item_0);

    QTableWidgetItem *item_1=new QTableWidgetItem(_tr("Description"));
    item_1->setFlags(Qt::ItemIsEnabled);
    setItem(1, 0, item_1);

    QTableWidgetItem *item_2=new QTableWidgetItem("λ");
    item_2->setFlags(Qt::ItemIsEnabled);
    setItem(2, 0, item_2);

    QTableWidgetItem *item_3=new QTableWidgetItem("ε");
    item_3->setFlags(Qt::ItemIsEnabled);
    setItem(3, 0, item_3);

    QTableWidgetItem *item_4 = new QTableWidgetItem("μ");
    item_4->setFlags(Qt::ItemIsEnabled);
    setItem(4, 0, item_4);

    QTableWidgetItem *item_5=new QTableWidgetItem(_tr("Color"));
    item_5->setFlags(Qt::ItemIsEnabled);
    setItem(5, 0, item_5);

    setItem(0, 1, new QTableWidgetItem);
    setItem(1, 1, new QTableWidgetItem);
    setItem(2, 1, new QTableWidgetItem);
    setItem(3, 1, new QTableWidgetItem);
    setItem(4, 1, new QTableWidgetItem);

    QTableWidgetItem *color_item=new QTableWidgetItem;
    color_item->setFlags(Qt::ItemIsEnabled);
    setItem(5, 1, color_item);

    setColumnWidth(0, 80);
    setColumnWidth(1, 105);

    setFixedWidth(187);

    setRowHeight(0, ROW_HEIGHT);
    setRowHeight(1, ROW_HEIGHT);
    setRowHeight(2, ROW_HEIGHT);
    setRowHeight(3, ROW_HEIGHT);
    setRowHeight(4, ROW_HEIGHT);
    setRowHeight(5, ROW_HEIGHT);

    setFixedHeight(LIST_HEIGHT);

    connect(this, &MaterialProperties::cellChanged, this,  &MaterialProperties::onCellChanged);
    connect(this, &MaterialProperties::cellDoubleClicked, this, &MaterialProperties::onCellDoubleClicked);

    hide();
}

void MaterialProperties::setIndent(int indent)
{
    setFixedWidth(187-indent*10);
    setColumnWidth(1, 105-indent*10);
}

MaterialProperties::~MaterialProperties()
{
}

void MaterialProperties::setMaterialId(const QString &id)
{
    _material=Materials::instance()->materialFromId(id.toStdString());
}

void MaterialProperties::setName(const QString &name)
{
    _catchUserInputs=false;

    QTableWidgetItem *item_0=item(0, 1);
    if (item_0)
        item_0->setText(name);

    _catchUserInputs=true;
}

void MaterialProperties::setEmissivity(double emissivity)
{
    _catchUserInputs=false;
    QTableWidgetItem *item_1=item(3, 1);
    QLocale locale;
    if (item_1)
        item_1->setText(locale.toString(emissivity));
    _catchUserInputs=true;
}

void MaterialProperties::setMu(double mu)
{
    _catchUserInputs=false;
    QTableWidgetItem *item_1=item(4, 1);
    QLocale locale;
    if (item_1)
        item_1->setText(locale.toString(mu));
    _catchUserInputs=true;
}

void MaterialProperties::setDescription(const QString &description)
{
    _catchUserInputs=false;

    QTableWidgetItem *item_0=item(1, 1);
    if (item_0)
        item_0->setText(description);

    _catchUserInputs=true;
}

void MaterialProperties::setConductivity(double conductivity)
{
    _catchUserInputs=false;
    QTableWidgetItem *item_1=item(2, 1);
    QLocale locale;
    if (item_1)
        item_1->setText(locale.toString(conductivity));
    _catchUserInputs=true;
}

void MaterialProperties::setColor(const QColor &color)
{
    _catchUserInputs=false;
    QTableWidgetItem *item_2=item(5, 1);
    if (item_2)
        item_2->setBackground(QBrush(color));
    _catchUserInputs=true;
}

void MaterialProperties::onCellChanged(int row, int column)
{
    if (!_catchUserInputs)
        return;

    if (!_material)
        return;

    // Name.
    if (row==0 && item(0, 1))
    {
        QString name=item(0, 1)->text();
        SetMaterialName *action=new SetMaterialName(_material, name.toStdString());
        ActionsManager::instance()->exec(action);
    }

    // Description.
    if (row==1 && item(1, 1))
    {
        QString description=item(1, 1)->text();
        SetMaterialDescription *action=new SetMaterialDescription(_material, description.toStdString());
        ActionsManager::instance()->exec(action);
    }

    // Conductivity.
    else if (row==2 && item(2, 1))
    {
        QLocale locale;

        QString data=item(2, 1)->text();

        data.replace(locale.groupSeparator(), "");
        data.replace(",", ".");

        double conductivity=data.toDouble();
        if (conductivity<0.001)
            conductivity=0.001;

        SetMaterialConductivity *action=new SetMaterialConductivity(_material, conductivity);
        ActionsManager::instance()->exec(action);

        _catchUserInputs=false;
        item(2, 1)->setText(locale.toString(conductivity));
        _catchUserInputs=true;
    }

    // Emissivity.
    else if (row==3 && item(3, 1))
    {
        QLocale locale;

        QString data=item(3, 1)->text();

        data.replace(locale.groupSeparator(), "");
        data.replace(",", ".");

        double emissivity=data.toDouble();
        if (emissivity<0.0)
            emissivity=0.0;
        if (emissivity>1.0)
            emissivity=1.0;

        SetMaterialEmissivity *action=new SetMaterialEmissivity(_material, emissivity);
        ActionsManager::instance()->exec(action);

        _catchUserInputs=false;
        item(3, 1)->setText(locale.toString(emissivity));
        _catchUserInputs=true;
    }

    // Emissivity.
    else if (row==4 && item(4, 1))
    {
        QLocale locale;

        QString data=item(4, 1)->text();

        data.replace(locale.groupSeparator(), "");
        data.replace(",", ".");

        double mu=data.toDouble();
        if (mu<0.0)
            mu=0.0;

        SetMaterialMu *action=new SetMaterialMu(_material, mu);
        ActionsManager::instance()->exec(action);

        _catchUserInputs=false;
        item(4, 1)->setText(locale.toString(mu));
        _catchUserInputs=true;
    }

    // Color.
    else if (row==5 && item(5, 1))
    {
        QColor color=item(5, 1)->background().color();
        SetMaterialColor *action=new SetMaterialColor(_material, color.red(), color.green(), color.blue());
        ActionsManager::instance()->exec(action);
    }
}

void MaterialProperties::onCellDoubleClicked(int row, int column)
{
    // Color.
    if (row==5 && item(5, 1))
    {
        // Keep current color for further use.
        QColor currentColor=item(5, 1)->background().color();

        QColorDialog dialog(this);
        dialog.setCurrentColor(currentColor);
        dialog.setWindowTitle(_tr("MaterialColor"));
        connect(&dialog,
                &QColorDialog::currentColorChanged,
                this,
                &MaterialProperties::onCurrentSelectedColorChanged);
        dialog.exec();
        QColor color=dialog.selectedColor();
        if (!color.isValid())
            item(5, 1)->setBackground(currentColor);
    }
}

void MaterialProperties::onCurrentSelectedColorChanged(const QColor &color)
{
    item(5, 1)->setBackground(color);
}

void MaterialProperties::translate()
{
    item(0, 0)->setText(_tr("Name"));
    item(1, 0)->setText(_tr("Description"));
    item(5, 0)->setText(_tr("Color"));
    update();
}
