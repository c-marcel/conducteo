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

#include <TranslationBoard.h>
#include <LinguistManager.h>
#include <QPainter>

TranslationBoard::TranslationBoard(Direction direction, QWidget *parent):
    QWidget(parent),
    _increase(this),
    _decrease(this),
    _direction(direction)
{
    _increase.setIconSize(QSize(28, 28));
    _decrease.setIconSize(QSize(28, 28));

    int w=100;
    int h=35;

    // Vertical.
    if (direction==Vertical)
    {
        _increase.setGeometry(0, 0, 32, 32);
        _decrease.setGeometry(0, 60, 32, 32);
        w=35;
        h=100;
    }

    // Horizontal.
    else if (direction==Horizontal)
    {
        _decrease.setGeometry(0, 0, 32, 32);
        _increase.setGeometry(60, 0, 32, 32);
    }

    setMinimumHeight(h);
    setMaximumHeight(h);
    setMinimumWidth(w);
    setMaximumWidth(w);

    applyTheme();

    connect(&_decrease, &QAbstractButton::clicked, this, &TranslationBoard::decreaseRequested);
    connect(&_increase, &QAbstractButton::clicked, this, &TranslationBoard::increaseRequested);
}

TranslationBoard::~TranslationBoard()
{
}

void TranslationBoard::applyTheme()
{
    if (_direction==Vertical)
    {
        _increase.setIcon(QIcon(":/images/orange-blue/up.png"));
        _decrease.setIcon(QIcon(":/images/orange-blue/down.png"));
    }

    else if (_direction==Horizontal)
    {
        _decrease.setIcon(QIcon(":/images/orange-blue/left.png"));
        _increase.setIcon(QIcon(":/images/orange-blue/right.png"));
    }
}
