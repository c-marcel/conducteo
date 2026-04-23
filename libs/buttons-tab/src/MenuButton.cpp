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

#include <MenuButton.h>
#include <QMenu>

MenuButton::MenuButton(QWidget *parent):
    QPushButton(parent),
    _menu(0),
    _useParentWidth(true)
{
#ifdef _WIN32
    _menu_area_width = 20;
    _menu_delta_x    = 0;
    _menu_delta_y    = -5;
#else
    _menu_area_width = 35;
    _menu_delta_x    = -80;
    _menu_delta_y    = -8;
#endif
}

MenuButton::~MenuButton()
{
}

void MenuButton::setMargins(int dx, int dy)
{
    _menu_delta_x = dx;
    _menu_delta_y = dy;
}

void MenuButton::installMenu(QMenu *menu)
{
    _menu = menu;
    updateTitle();
}

void MenuButton::setTitle(const QString &title)
{
    _title = title;
    updateTitle();
}

void MenuButton::useParentWidth(bool use)
{
    _useParentWidth = use;
}

bool MenuButton::hitButton(const QPoint &pos) const
{
    QPushButton *mthis = (QPushButton*) this;

    // Right: display menu.
    if (pos.x() > width() - _menu_area_width && _menu)
    {
        QPoint p = mapToGlobal(QPoint(x() + (_useParentWidth ? - parentWidget()->x() : 0) + _menu_delta_x, y() + height() + _menu_delta_y));
        _menu->move(p);
        _menu->show();
    }

    // Left: emit click.
    else
        mthis->clicked(true);

    return false;
}

void MenuButton::updateTitle()
{
    if (_menu)
        setText(_title + " ▼");
    else
        setText(_title);
}
