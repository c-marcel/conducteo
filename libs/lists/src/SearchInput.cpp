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

#include <SearchInput.h>
#include <LinguistManager.h>
#include <QMouseEvent>
#include <QPainter>

SearchInput::SearchInput(Qt::Orientation o, QWidget *parent):
    CustomHeaderView(o, parent),
    _edit(this)
{
    _edit.setGeometry(40, 55, 210, 20);

    connect(&_edit,
            &QLineEdit::textEdited,
            this,
            &SearchInput::searchInputTextChanged,
            Qt::UniqueConnection);

    _image=QPixmap(":/images/orange-blue/search.png").scaledToHeight(20, Qt::SmoothTransformation);
}

SearchInput::~SearchInput()
{
}

QSize SearchInput::sizeHint() const
{
    return QSize(200, 80);
}

void SearchInput::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QRect r=rect;
    r.setHeight(50);
    CustomHeaderView::paintSection(painter, r, logicalIndex);

    // Draw image.
    int left=10;
    int top=50+(rect.height()-50-_image.height())/2;
    QRect r2(left, top, _image.width(), _image.height());
    painter->drawPixmap(r2, _image);
}

void SearchInput::mouseReleaseEvent(QMouseEvent *event)
{
    int left=10;
    int top=50+(height()-50-_image.height())/2;
    QRect r2(left, top, _image.width(), _image.height());

    if (r2.contains(event->pos()))
    {
        _edit.setText("");
        emit searchInputTextChanged("");
    }
}
