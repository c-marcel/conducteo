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

#include <computation/ValidationsThread.h>
#include <QDesktopServices>
#include <QStandardPaths>
#include <tests/Tests.h>
#include <QFile>
#include <QUrl>

ValidationsThread::ValidationsThread()
{
    moveToThread(this);
}

ValidationsThread::~ValidationsThread()
{
}

void ValidationsThread::run()
{
    // Compute tests.
    Tests tests;
    std::string report=tests.exec();

    // Save & open report.
    QString filename=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+"/conducteo_report.txt";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(report.c_str());
    file.close();

    QDesktopServices::openUrl(QUrl("file:///"+filename));
}
