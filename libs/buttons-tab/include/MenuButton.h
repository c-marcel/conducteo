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

#ifndef _MENU_BUTTON_H
#define _MENU_BUTTON_H

#include <QPushButton>

class MenuButton: public QPushButton
{
    Q_OBJECT
public:
    MenuButton(QWidget *parent=0);
    virtual ~MenuButton();

    void installMenu(QMenu *menu);
    void setTitle(const QString& title);
    void useParentWidth(bool use);

    void setMargins(int dx, int dy);

protected:
    bool hitButton(const QPoint & pos) const;

private:
    void updateTitle();

    QMenu  *_menu;
    QString _title;
    bool    _useParentWidth;

    int     _menu_area_width;
    int     _menu_delta_x;
    int     _menu_delta_y;
};

#endif
