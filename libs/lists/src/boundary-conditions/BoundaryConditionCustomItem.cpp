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

#include <boundary-conditions/BoundaryConditionCustomItem.h>
#include <boundary-conditions/BoundaryConditionProperties.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>
#include <StatesManager.h>
#include <QPainter>
#include <QLocale>

#define ItemWidth                       200
#define ClosedItemHeight                25

#ifdef _WIN64
#define ExpandedItemAdiabaticHeight     100
#define ExpandedItemDefaultHeight       135
#endif

#ifdef __APPLE__
#define ExpandedItemAdiabaticHeight     70
#define ExpandedItemDefaultHeight       90
#endif

#ifdef __linux__
#define ExpandedItemAdiabaticHeight     70
#define ExpandedItemDefaultHeight       90
#endif

BoundaryConditionCustomItem::BoundaryConditionCustomItem():
    _temperature(0.0),
    _resistance(0.0),
    _properties(0),
    _expanded(false),
    _editable(false),
    _rh(0.8)
{
    setSizeHint(0, QSize(ItemWidth, ClosedItemHeight));
}

BoundaryConditionCustomItem::~BoundaryConditionCustomItem()
{
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(_id.toStdString());
    if (bc)
        bc->unregisterObserver(this);
}

void BoundaryConditionCustomItem::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);
    QRect rect=geometry();

    // Draw rectangle.
    QBrush brush(_color);
    qp.setBrush(brush);
    QRect r(4, 2, 30, 20);
    qp.drawRect(r);
    
    // Draw text.
    QFont f = qp.font();
    f.setPixelSize(12);
    qp.setFont(f);

    qp.drawText(r.width()+10, 0, rect.width()-r.width(), 25, Qt::AlignVCenter, _translatedTitle);
}

void BoundaryConditionCustomItem::setBoundaryCondition(BoundaryCondition *bc)
{
    bc->registerObserver(this);
    _id=bc->id().c_str();
}

void BoundaryConditionCustomItem::setTitle(const QString &title)
{
    _title=title;
    translate();
}

void BoundaryConditionCustomItem::setColor(const QColor &color)
{
    _color=color;
}

void BoundaryConditionCustomItem::setTemperature(double temperature)
{
    _temperature = temperature;

    updateTooltip();
}

void BoundaryConditionCustomItem::setResistance(double resistance)
{
    _resistance = resistance;

    updateTooltip();
}

void BoundaryConditionCustomItem::setRh(double rh)
{
    _rh = rh;

    updateTooltip();
}

void BoundaryConditionCustomItem::setEditable(bool editable)
{
    _editable=editable;
}

QString BoundaryConditionCustomItem::id() const
{
    return _id;
}

QString BoundaryConditionCustomItem::title() const
{
    return _title;
}

void BoundaryConditionCustomItem::expandItem(QTreeWidgetItem *item)
{
    if (item==this && _expanded)
        _expanded=false;
    else if (item==this)
        _expanded=true;
    else
        _expanded=false;

    if (_expanded && _resistance!=InfiniteResistance)
        setSizeHint(0, QSize(ItemWidth, ExpandedItemDefaultHeight));
    else if (_expanded)
        setSizeHint(0, QSize(ItemWidth, ExpandedItemAdiabaticHeight));
    else
        setSizeHint(0, QSize(ItemWidth, ClosedItemHeight));

    setExpanded(_expanded);
}

void BoundaryConditionCustomItem::modelAltered()
{
    // Do nothing
}

void BoundaryConditionCustomItem::modelResultsAltered()
{
    // Do nothing
}

void BoundaryConditionCustomItem::changed()
{
    BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(_id.toStdString());
    if (!bc)
        return;

    _title=bc->name().c_str();
    _color=QColor(bc->red(), bc->green(), bc->blue());
    _resistance=bc->resistance();
    _temperature=bc->temperature();
    _rh = bc->rh();

    translate();

    if (_properties)
    {
        _properties->setName(_title);
        _properties->setTemperature(_temperature);
        _properties->setResistance(_resistance);
        _properties->setRh(_rh);
        _properties->setColor(_color);
    }
    repaint();

    emit saveDatabaseRequested();
}

void BoundaryConditionCustomItem::setExpanded(bool expanded)
{
    if (expanded)
    {
        if (!_properties)
            _properties=new BoundaryConditionProperties(this);
        _properties->setGeometry(40, 26, width()-46, 46);
        _properties->setBoundaryConditionId(_id);
        _properties->setName(_title);
        _properties->setResistance(_resistance);
        _properties->setRh(_rh);
        _properties->setTemperature(_temperature);
        _properties->setColor(_color);
        _properties->setEditable(_editable);
        _properties->show();
    }
    else if (_properties)
    {
        delete _properties;
        _properties=0;
    }
}

void BoundaryConditionCustomItem::updateTooltip()
{
    QLocale locale;
    QString text = "T=" + locale.toString(_temperature) + "°C, R=" + locale.toString(_resistance) + " m².°C/W, RH=" + locale.toString(int(_rh * 100)) + "%";

    QWidget::setToolTip(text);
}

bool BoundaryConditionCustomItem::operator<(const QTreeWidgetItem &other) const
{
    const BoundaryConditionCustomItem *bc=dynamic_cast<const BoundaryConditionCustomItem*>(&other);
    if (!bc)
        return false;
    return (_title<bc->title());
}

void BoundaryConditionCustomItem::translate()
{
    // Try to translate.
    QString t=_tr(_title);
    if (!t.isEmpty())
        _translatedTitle=t;
    else
        _translatedTitle=_title;

    if (_properties)
        _properties->setName(_translatedTitle);
    repaint();
}
