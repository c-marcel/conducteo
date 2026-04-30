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

#include <VolumeProperties.h>
#include <actions/model/UpdateRectangleGeometry.h>
#include <ComboBoxMaterialDelegate.h>
#include <actions/ActionsManager.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <model/Volume.h>
#include <QPainter>

VolumeProperties::VolumeProperties(QWidget *parent):
    QWidget(parent),
    _layout(this)
{
    QFont f=font();
    f.setBold(true);
    f.setPixelSize(12);

    QSpacerItem *vS1=new QSpacerItem(20, 60, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _layout.addItem(vS1);

    _layout.setContentsMargins(20, 10, 15, 0);

    _geometryTitle.setFont(f);
    _layout.addWidget(&_geometryTitle);

    _labelUnit_1.setText("m");
    _labelUnit_1.setAlignment(Qt::AlignVCenter);
    _labelUnit_1.setMaximumHeight(10);

    _labelUnit_2.setText("m");
    _labelUnit_2.setAlignment(Qt::AlignVCenter);
    _labelUnit_2.setMaximumHeight(10);

    _labelUnit_3.setText("m");
    _labelUnit_3.setAlignment(Qt::AlignVCenter);
    _labelUnit_3.setMaximumHeight(10);

    _labelUnit_4.setText("m");
    _labelUnit_4.setAlignment(Qt::AlignVCenter);
    _labelUnit_4.setMaximumHeight(10);

    _labelsLayout.addWidget(&_labelUnit_1);
    _labelsLayout.addWidget(&_labelUnit_2);
    _labelsLayout.addWidget(&_labelUnit_3);
    _labelsLayout.addWidget(&_labelUnit_4);

    _geometryLayout.setContentsMargins(15, 0, 0, 0);
    _geometryLayout.addWidget(&_geometryData, 0, Qt::AlignLeft);
    _geometryLayout.addLayout(&_labelsLayout);
    _layout.addLayout(&_geometryLayout);

    _materialTitle.setFont(f);
    _material.setItemDelegate(new ComboBoxMaterialDelegate(&_material, this));
    _layout.addWidget(&_materialTitle);
    _layout.addWidget(&_material);

    QSpacerItem *vS3=new QSpacerItem(0, 3, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _layout.addItem(vS3);

    _meshTitle.setFont(f);
    _layout.addWidget(&_meshTitle);
    _layout.addWidget(&_meshData);

    QSpacerItem *vS2=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS2);

    setFixedWidth(290);
    setMaximumHeight(275);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    connect(&_geometryData,
            &RectangleGeometricalProperties::dataXChanged,
            this,
            &VolumeProperties::dataXChanged);
    connect(&_geometryData,
            &RectangleGeometricalProperties::dataYChanged,
            this,
            &VolumeProperties::dataYChanged);
    connect(&_geometryData,
            &RectangleGeometricalProperties::dataWidthChanged,
            this,
            &VolumeProperties::dataWidthChanged);
    connect(&_geometryData,
            &RectangleGeometricalProperties::dataHeightChanged,
            this,
            &VolumeProperties::dataHeightChanged);

    translate();
    applyTheme();
}

VolumeProperties::~VolumeProperties()
{
}

void VolumeProperties::paintEvent(QPaintEvent *event)
{
    // Background.
    QPainter qp(this);
    QRect rec(9, 9, width()-18, height()-18);
    QBrush brush(Qt::white);
    qp.setBrush(brush);
    QPen pen;
    pen.setColor(Qt::darkGray);
    qp.setPen(pen);
    qp.drawRect(rec);

    // Header background.
    QRect rec2(10, 10, width()-19, 50);

    QColor themedColor = QColor(199, 229, 245);

    QBrush brush2(themedColor);
    qp.setBrush(brush2);
    pen.setColor(QColor(255, 255, 255, 0));
    qp.setPen(pen);
    qp.drawRect(rec2);

    // Draw image.
    QPixmap pixmap(":/images/orange-blue/add_rectangle.png");
    int left=10;
    int top=(rec2.height()-pixmap.height())/2;
    QRect r(10+left, 10+top, pixmap.width(), pixmap.height());
    qp.drawPixmap(r, pixmap);

    // Draw text.
    QFont f=font();
    f.setBold(true);
    f.setPixelSize(12);
    qp.setFont(f);
    qp.setPen(Qt::black);
    qp.drawText(10+2*left+pixmap.width(), 10, rec2.width()-2*left-pixmap.width(), rec2.height(), Qt::AlignVCenter, _title);
}

void VolumeProperties::translate()
{
    _materialTitle.setText(_tr("Material"));
    _meshTitle.setText(_tr("Mesh"));
    _geometryTitle.setText(_tr("Geometry"));
    _title=_tr("Volume");
    if (!_material.isVisible())
        _title=_tr("Meshes");
    repaint();
}

void VolumeProperties::statesChanged()
{
    StatesManager::ViewType view=StatesManager::instance()->viewType();
    if (view!=StatesManager::DisplayModel && view!=StatesManager::Display1DModel)
        return hide();

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.empty())
        return hide();

    show();

    _materialTitle.setVisible((volumes.size()==1));
    _material.setVisible((volumes.size()==1));

    _title=_tr("Volume");
    if (!_material.isVisible())
        _title=_tr("Maillages");

    // Volume is a rectangle.
    bool rectangle=false;
    if (volumes.size()==1)
    {
        Volume *v=volumes.at(0);
        rectangle=v->isRectangle();
        _geometryData.setData(v->xMin(), v->yMin(), v->xMax()-v->xMin(), v->yMax()-v->yMin());
    }

    _geometryTitle.setVisible(rectangle);
    _geometryData.setVisible(rectangle);
    _labelUnit_1.setVisible(rectangle);
    _labelUnit_2.setVisible(rectangle);
    _labelUnit_3.setVisible(rectangle);
    _labelUnit_4.setVisible(rectangle);

#ifdef __APPLE__
    int h=210;
#else
    int h=195;
#endif
    if (_material.isVisible())
#ifdef __APPLE__
        h=260;
#else
        h=240;
#endif
    if (rectangle)
#ifdef __APPLE__
        h=355;
#else
        h=325;
#endif
    setFixedHeight(h);

    repaint();
}

void VolumeProperties::dataXChanged(double x)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.size()!=1)
        return;

    Volume *volume=volumes.at(0);
    double xold=volume->xMin();
    double y=volume->yMin();
    double width=volume->xMax()-volume->xMin();
    double height=volume->yMax()-volume->yMin();

    if (x==xold)
        return;

    UpdateRectangleGeometry *action=new UpdateRectangleGeometry(model, volume, x, y, width, height);
    ActionsManager::instance()->exec(action);
}

void VolumeProperties::dataYChanged(double y)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.size()!=1)
        return;

    Volume *volume=volumes.at(0);
    double x=volume->xMin();
    double yold=volume->yMin();
    double width=volume->xMax()-volume->xMin();
    double height=volume->yMax()-volume->yMin();

    if (y==yold)
        return;

    UpdateRectangleGeometry *action=new UpdateRectangleGeometry(model, volume, x, y, width, height);
    ActionsManager::instance()->exec(action);
}

void VolumeProperties::dataWidthChanged(double width)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.size()!=1)
        return;

    Volume *volume=volumes.at(0);
    double x=volume->xMin();
    double y=volume->yMin();
    double widthold=volume->xMax()-volume->xMin();
    double height=volume->yMax()-volume->yMin();

    if (width==widthold)
        return;

    UpdateRectangleGeometry *action=new UpdateRectangleGeometry(model, volume, x, y, width, height);
    ActionsManager::instance()->exec(action);
}

void VolumeProperties::dataHeightChanged(double height)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.size()!=1)
        return;

    Volume *volume=volumes.at(0);
    double x=volume->xMin();
    double y=volume->yMin();
    double width=volume->xMax()-volume->xMin();
    double heightold=volume->yMax()-volume->yMin();

    if (heightold==height)
        return;

    UpdateRectangleGeometry *action=new UpdateRectangleGeometry(model, volume, x, y, width, height);
    ActionsManager::instance()->exec(action);
}
