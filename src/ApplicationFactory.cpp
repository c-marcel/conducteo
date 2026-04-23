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

#include <ApplicationFactory.h>
#include <MainWindow.h>

MainWindow *createMainWindow()
{
    MainWindow *window=new MainWindow;
    return window;
}

void deleteWindow(MainWindow *window)
{
    delete window;
}

void loadMainWindowFile(MainWindow *window, const char *filename)
{
#ifdef WIN32
    QString f=QString::fromLatin1(filename);
    window->loadModel(f.toStdString());
#else
    window->loadModel(filename);
#endif
}
