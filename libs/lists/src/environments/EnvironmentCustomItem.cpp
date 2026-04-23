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

#include <environments/EnvironmentCustomItem.h>
#include <physics/Environment.h>
#include <QPainter>
#include <QLocale>

EnvironmentCustomItem::EnvironmentCustomItem()
{
    setSizeHint(0, QSize(200, 25));
}

EnvironmentCustomItem::~EnvironmentCustomItem()
{
}

void EnvironmentCustomItem::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);
    QRect rect = geometry();

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

void EnvironmentCustomItem::setTitle(const QString &title)
{
    _title=title;
    translate();
    repaint();
}

void EnvironmentCustomItem::setId(const QString &id)
{
    _id=id;
}

void EnvironmentCustomItem::setColor(const QColor &color)
{
    _color=color;
    repaint();
}

QString EnvironmentCustomItem::id() const
{
    return _id;
}

QString EnvironmentCustomItem::title() const
{
    return _title;
}

void EnvironmentCustomItem::translate()
{
    _translatedTitle = _tr(_title);

    if (_translatedTitle.isEmpty())
        _translatedTitle = _title;

    repaint();
}
