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

#ifndef _CUSTOM_HEADER_VIEW_H
#define _CUSTOM_HEADER_VIEW_H

#include <QHeaderView>

class CustomHeaderView: public QHeaderView
{

public:

    CustomHeaderView(Qt::Orientation o, QWidget *parent=0);
    virtual ~CustomHeaderView();

    void setBackgroundColor(const QColor &color);
    void setTitle(const QString &title);
    void setIcon(const QPixmap &pixmap);

    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    virtual QSize sizeHint() const;

private:

    QString     _title;
    QPixmap     _pixmap;
    QColor      _background;

};

#endif
