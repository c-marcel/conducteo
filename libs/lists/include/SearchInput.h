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

#ifndef _SEARCH_INPUT_H
#define _SEARCH_INPUT_H

#include <CustomHeaderView.h>
#include <Translatable.h>
#include <QLineEdit>

class SearchInput: public CustomHeaderView,
                   public Translatable
{

    Q_OBJECT

public:

    SearchInput(Qt::Orientation o, QWidget *parent=0);
    virtual ~SearchInput();

    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    virtual QSize sizeHint() const;

protected:

    void mouseReleaseEvent(QMouseEvent *event);

signals:

    void searchInputTextChanged(const QString &text);

private:

    QLineEdit   _edit;
    QPixmap     _image;

};

#endif
