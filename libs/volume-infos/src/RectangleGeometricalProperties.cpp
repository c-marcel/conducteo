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

#include <RectangleGeometricalProperties.h>
#include <QColorDialog>
#include <QHeaderView>
#include <QLocale>

#define ROW_HEIGHT 12

#ifdef _WIN64
#define ListHeight 83
#endif

#ifdef __APPLE__
#define ListHeight 52
#endif

#ifdef __linux__
#define ListHeight 52
#endif

RectangleGeometricalProperties::RectangleGeometricalProperties(QWidget *parent):
    QTableWidget(parent),
    _catchUserInputs(true)
{
    QFont f = font();
    f.setPixelSize(10);
    setFont(f);

    setRowCount(4);
    setColumnCount(2);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
    setRowHeight(0, ROW_HEIGHT);
    setRowHeight(1, ROW_HEIGHT);
    setRowHeight(2, ROW_HEIGHT);
    setRowHeight(3, ROW_HEIGHT);
    
    QTableWidgetItem *item_0=new QTableWidgetItem("X");
    item_0->setFlags(Qt::ItemIsEnabled);
    setItem(0, 0, item_0);

    QTableWidgetItem *item_1=new QTableWidgetItem("Y");
    item_1->setFlags(Qt::ItemIsEnabled);
    setItem(1, 0, item_1);

    QTableWidgetItem *item_2=new QTableWidgetItem(_tr("Width"));
    item_2->setFlags(Qt::ItemIsEnabled);
    setItem(2, 0, item_2);

    QTableWidgetItem *item_3=new QTableWidgetItem(_tr("Height"));
    item_3->setFlags(Qt::ItemIsEnabled);
    setItem(3, 0, item_3);

    setItem(0, 1, new QTableWidgetItem);
    setItem(1, 1, new QTableWidgetItem);
    setItem(2, 1, new QTableWidgetItem);
    setItem(3, 1, new QTableWidgetItem);

    setColumnWidth(0, 60);
    setColumnWidth(1, 100);

    setMaximumWidth(161);
    setFixedHeight(ListHeight);

    connect(this, &RectangleGeometricalProperties::cellChanged, this, &RectangleGeometricalProperties::onCellChanged);
}

RectangleGeometricalProperties::~RectangleGeometricalProperties()
{
}

void RectangleGeometricalProperties::setData(const double x, const double y, const double width, const double height)
{
    QLocale locale;
    _catchUserInputs=false;

    QTableWidgetItem *item_0=item(0, 1);
    if (item_0)
        item_0->setText(locale.toString(x, 'f', 4));

    QTableWidgetItem *item_1=item(1, 1);
    if (item_1)
        item_1->setText(locale.toString(y, 'f', 4));

    QTableWidgetItem *item_2=item(2, 1);
    if (item_2)
        item_2->setText(locale.toString(width, 'f', 4));

    QTableWidgetItem *item_3=item(3, 1);
    if (item_3)
        item_3->setText(locale.toString(height, 'f', 4));

    _catchUserInputs=true;
}

void RectangleGeometricalProperties::onCellChanged(int row, int column)
{
    if (!_catchUserInputs)
        return;

    QLocale locale;

    // X.
    if (row==0 && item(0, 1))
    {
        QString data=item(0, 1)->text();
        data.replace(",", ".");
        double x=data.toDouble();
        emit dataXChanged(x);
        _catchUserInputs=false;
        item(0, 1)->setText(locale.toString(x, 'f', 4));
        _catchUserInputs=true;
    }

    // Y.
    else if (row==1 && item(1, 1) && item(3, 1))
    {
        QString data=item(1, 1)->text();
        data.replace(",", ".");
        double y=data.toDouble();

        emit dataYChanged(y);
        _catchUserInputs=false;
        item(1, 1)->setText(locale.toString(y, 'f', 4));
        _catchUserInputs=true;
    }

    // Width.
    else if (row==2 && item(2, 1))
    {
        QString data=item(2, 1)->text();
        data.replace(",", ".");
        double width=data.toDouble();
        if (width<0.001)
            width=0.001;
        emit dataWidthChanged(width);
        _catchUserInputs=false;
        item(2, 1)->setText(locale.toString(width, 'f', 4));
        _catchUserInputs=true;
    }

    // Height.
    else if (row==3 && item(3, 1) && item(1, 1))
    {
        QString data2=item(3, 1)->text();
        data2.replace(",", ".");
        double height=data2.toDouble();
        if (height<0.001)
            height=0.001;

        emit dataHeightChanged(height);
        _catchUserInputs=false;
        item(3, 1)->setText(locale.toString(height, 'f', 4));
        _catchUserInputs=true;
    }
}

void RectangleGeometricalProperties::translate()
{
    _catchUserInputs=false;
    item(2, 0)->setText(_tr("Width"));
    item(3, 0)->setText(_tr("Height"));
    _catchUserInputs=true;
    update();
}
