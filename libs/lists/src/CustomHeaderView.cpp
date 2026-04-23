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

#include <CustomHeaderView.h>
#include <QApplication>
#include <QPainter>

CustomHeaderView::CustomHeaderView(Qt::Orientation o, QWidget *parent):
    QHeaderView(o, parent)
{
    setSectionResizeMode(Stretch);
}

CustomHeaderView::~CustomHeaderView()
{
}

QSize CustomHeaderView::sizeHint() const
{
    return QSize(200, 50);
}

void CustomHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // Draw background.
    painter->fillRect(rect, _background);

    // Draw image.
    int left=10;
    int top=(rect.height()-_pixmap.height())/2;
    QRect r(left, top, _pixmap.width(), _pixmap.height());
    painter->drawPixmap(r, _pixmap);
    
    // Draw text.
    QFont f=QApplication::font();

    f.setBold(true);
    f.setPixelSize(12);
    painter->setFont(f);
    painter->drawText(2*left+_pixmap.width(), 0, rect.width()-2*left-_pixmap.width(), rect.height(), Qt::AlignVCenter, _title);
}

void CustomHeaderView::setTitle(const QString &title)
{
    _title=title;
    setFocus();
}

void CustomHeaderView::setIcon(const QPixmap &pixmap)
{
    _pixmap=pixmap;
    update();
}

void CustomHeaderView::setBackgroundColor(const QColor &color)
{
    _background=color;
    setVisible(false);
    update();
    setVisible(true);
    update();
}
