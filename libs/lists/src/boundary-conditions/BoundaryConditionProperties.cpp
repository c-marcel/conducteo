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

#include <boundary-conditions/BoundaryConditionProperties.h>
#include <actions/boundary-conditions/SetBoundaryConditionDescription.h>
#include <actions/boundary-conditions/SetBoundaryConditionTemperature.h>
#include <actions/boundary-conditions/SetBoundaryConditionResistance.h>
#include <actions/boundary-conditions/SetBoundaryConditionColor.h>
#include <actions/boundary-conditions/SetBoundaryConditionName.h>
#include <actions/boundary-conditions/SetBoundaryConditionRh.h>
#include <databases/BoundaryConditions.h>
#include <actions/ActionsManager.h>
#include <StatesManager.h>
#include <QColorDialog>
#include <QHeaderView>
#include <QLocale>

#define ROW_HEIGHT 12

#ifdef _WIN64
#define ListHeightAdiabatic 64
#define ListHeight          105
#endif

#ifdef __APPLE__
#define ListHeightAdiabatic 38
#define ListHeight          62
#endif

#ifdef __linux__
#define ListHeightAdiabatic 38
#define ListHeight          62
#endif

BoundaryConditionProperties::BoundaryConditionProperties(QWidget *parent):
    QTableWidget(parent),
    _catchUserInputs(true),
    _editable(true),
    _boundaryCondition(0)
{
    QFont f = font();
    f.setPixelSize(10);
    setFont(f);

    connect(this, &BoundaryConditionProperties::cellChanged, this, &BoundaryConditionProperties::cellChanged, Qt::UniqueConnection);
    connect(this, &BoundaryConditionProperties::cellDoubleClicked, this, &BoundaryConditionProperties::cellDoubleClicked, Qt::UniqueConnection);
}

BoundaryConditionProperties::~BoundaryConditionProperties()
{
}

void BoundaryConditionProperties::setName(const QString &name)
{
    _name=name;
    createGrid();
}

void BoundaryConditionProperties::setBoundaryConditionId(const QString &id)
{
    _boundaryCondition=BoundaryConditions::instance()->bcFromId(id.toStdString());
}

void BoundaryConditionProperties::setTemperature(double temperature)
{
    _temperature=temperature;
    createGrid();
}

void BoundaryConditionProperties::setResistance(double resistance)
{
    _resistance=resistance;
    createGrid();
}

void BoundaryConditionProperties::setRh(double rh)
{
    _rh = rh;
    createGrid();
}

void BoundaryConditionProperties::setColor(const QColor &color)
{
    _color=color;
    createGrid();
}

void BoundaryConditionProperties::cellChanged(int row, int column)
{
    if (!_catchUserInputs)
        return;
    if (!_editable)
        return;

    // Name.
    if (row==0 && item(0, 1))
    {
        QString name=item(0, 1)->text();
        SetBoundaryConditionName *action=new SetBoundaryConditionName(_boundaryCondition, name.toStdString());
        ActionsManager::instance()->exec(action);
    }

    // Temperature.
    else if (row==1 && item(1, 1))
    {
        QLocale locale;

        QString data=item(1, 1)->text();

        data.replace(locale.groupSeparator(), "");
        data.replace(",", ".");

        double temperature=data.toDouble();
        
        SetBoundaryConditionTemperature *action=new SetBoundaryConditionTemperature(_boundaryCondition, temperature);
        ActionsManager::instance()->exec(action);
        
        _catchUserInputs=false;
        item(1, 1)->setText(locale.toString(temperature));
        _catchUserInputs=true;
    }

    // Resistance.
    else if (row==2 && item(2, 1))
    {
        QLocale locale;

        QString data=item(2, 1)->text();

        data.replace(locale.groupSeparator(), "");
        data.replace(",", ".");

        double resistance=data.toDouble();
        if (resistance<0.001)
            resistance=0.001;

        SetBoundaryConditionResistance *action=new SetBoundaryConditionResistance(_boundaryCondition, resistance);
        ActionsManager::instance()->exec(action);

        _catchUserInputs=false;
        item(2, 1)->setText(locale.toString(resistance));
        _catchUserInputs=true;
    }

    // Humidity.
    else if (row==3 && item(3, 1))
    {
        QLocale locale;

        QString data=item(3, 1)->text();

        data.replace(locale.groupSeparator(), "");
        data.replace(",", ".");

        double rh = data.toDouble() / 100.0;

        if (rh < 0.0)
            rh=0.0;

        if (rh > 1.0)
            rh = 1.0;

        SetBoundaryConditionRh *action=new SetBoundaryConditionRh(_boundaryCondition, rh);
        ActionsManager::instance()->exec(action);

        _catchUserInputs=false;
        item(3, 1)->setText(locale.toString(int(rh * 100.0)));
        _catchUserInputs=true;
    }

    // Color.
    else if (row==4 && item(4, 1))
    {
        QColor color=item(4, 1)->background().color();
        SetBoundaryConditionColor *action=new SetBoundaryConditionColor(_boundaryCondition, color.red(), color.green(), color.blue());
        ActionsManager::instance()->exec(action);
    }
}

void BoundaryConditionProperties::cellDoubleClicked(int row, int column)
{
    if (!_editable)
        return;

    // Color.
    if (row==4 && item(4, 1))
    {
        // Keep current color for further use.
        QColor currentColor=item(4, 1)->background().color();

        QColorDialog dialog(this);
        dialog.setCurrentColor(currentColor);
        dialog.setWindowTitle(_tr("BCColor"));
        connect(&dialog,
                &QColorDialog::currentColorChanged,
                this,
                &BoundaryConditionProperties::currentSelectedColorChanged);
        dialog.exec();
        QColor color=dialog.selectedColor();
        if (!color.isValid())
            item(4, 1)->setBackground(currentColor);
    }
}

void BoundaryConditionProperties::currentSelectedColorChanged(const QColor &color)
{
    item(4, 1)->setBackground(color);
}

void BoundaryConditionProperties::setEditable(bool editable)
{
    Qt::ItemFlags flags=Qt::ItemIsEnabled;
    if (editable)
        flags=Qt::ItemIsEnabled|Qt::ItemIsEditable;

    _catchUserInputs=false;
    if (item(0, 1))
        item(0, 1)->setFlags(flags);
    if (item(1, 1))
        item(1, 1)->setFlags(flags);
    if (item(2, 1))
        item(2, 1)->setFlags(flags);
    if (item(3, 1))
        item(3, 1)->setFlags(flags);
    _catchUserInputs=true;
    
    _editable=editable;
}

void BoundaryConditionProperties::createGrid()
{
    _catchUserInputs=false;

    QLocale locale;

    setRowCount(5);

    int nrow = 3;

    if (_resistance > 0.0)
    {
        nrow += 1;
        nrow += 1;
    }

    setColumnCount(2);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);

    setRowHeight(0, ROW_HEIGHT);

    if (_resistance> 0.0)
        setRowHeight(1, ROW_HEIGHT);
    else
        setRowHeight(1, 0);

    setRowHeight(2, ROW_HEIGHT);

    if (_resistance> 0.0)
        setRowHeight(3, ROW_HEIGHT);
    else
        setRowHeight(3, 0);

    setRowHeight(4, ROW_HEIGHT);

    int idx=0;
    QTableWidgetItem *item_0=new QTableWidgetItem(_tr("Name"));
    item_0->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 0, item_0);

    item_0=new QTableWidgetItem();
    item_0->setText(_name);
    item_0->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 1, item_0);
    idx++;

    QTableWidgetItem *item_1=new QTableWidgetItem;

    if (_resistance > 0.0)
        item_1->setText(_tr("Temperature"));

    item_1->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 0, item_1);

    item_1=new QTableWidgetItem;

    if (_resistance > 0.0)
        item_1->setText(locale.toString(_temperature));

    setItem(idx, 1, item_1);
    idx++;

    QTableWidgetItem *item_2=new QTableWidgetItem(_tr("Resistance"));
    item_2->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 0, item_2);

    item_2=new QTableWidgetItem;
    if (_resistance>0.0)
        item_2->setText(locale.toString(_resistance));
    else
        item_2->setText("∞");
    setItem(idx, 1, item_2);
    idx++;

    QTableWidgetItem *item_3=new QTableWidgetItem();

    if (_resistance > 0.0)
        item_3->setText(_tr("Temperature"));

    item_3->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 0, item_3);

    item_3=new QTableWidgetItem;

    if (_resistance > 0.0)
        item_3->setText(locale.toString(_rh * 100.0));

    setItem(idx, 1, item_3);
    idx++;
    
    QTableWidgetItem *item_4=new QTableWidgetItem(_tr("Color"));
    item_4->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 0, item_4);

    item_4=new QTableWidgetItem;
    item_4->setBackground(QBrush(_color));
    item_4->setFlags(Qt::ItemIsEnabled);
    setItem(idx, 1, item_4);
    idx++;

    setColumnWidth(0, 75);
    setColumnWidth(1, 93);

    if (nrow == 3)
        setGeometry(40, 26, 170, ListHeightAdiabatic);
    else
        setGeometry(40, 26, 170, ListHeight);

    translate();

    _catchUserInputs=true;
}

void BoundaryConditionProperties::translate()
{
    bool useCondensation = true;

    QString t=_tr(_name);
    if (!t.isEmpty())
        item(0, 1)->setText(t);
    else
        item(0, 1)->setText(_name);

    item(0, 0)->setText(_tr("Name"));

    if (_resistance > 0.0)
        item(1, 0)->setText(_tr("Temperature"));
    else
        item(1, 0)->setText("");

    item(2, 0)->setText(_tr("Resistance"));

    if (useCondensation && _resistance > 0.0)
        item(3, 0)->setText(_tr("Humidity"));
    else
        item(3, 0)->setText("");

    item(4, 0)->setText(_tr("Color"));

    update();
}
