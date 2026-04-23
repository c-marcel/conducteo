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

#include <databases/BoundaryConditions.h>
#include <actions/ActionsManager.h>
#include <databases/Environments.h>
#include <databases/Materials.h>
#include <ApplicationFactory.h>
#include <LinguistManager.h>
#include <QDesktopServices>
#include <StatesManager.h>
#include <QStandardPaths>
#include <QApplication>
#include <LocalData.h>
#include <QFileInfo>
#include <time.h>
#include <Log.h>

#ifdef __APPLE__
    #include <CoreFoundation/CoreFoundation.h>
    #include <QFont>
    #include <QDir>
#endif

#ifdef WIN32
#pragma comment(linker, "/SUBSYSTEM:windows")
#include <Windows.h>
#define argc __argc
#define argv __argv
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
#ifdef __APPLE__
    CFURLRef appUrlRef=CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath=CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr=CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
    CFRelease(appUrlRef);
    CFRelease(macPath);
    QDir::setCurrent(QString(pathPtr)+"/Contents/MacOs/");
#endif

    // Set log.
#ifdef _WIN32    
    QString desktopPath=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    GETLOG->setLogFileName(desktopPath.toLatin1().toStdString() + "/conducteo.log");
    GETLOG->setLogLevel(Log::LTRACE);
#else
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    GETLOG->setLogFileName(desktopPath.toStdString() + "/conducteo.log");
    GETLOG->setLogLevel(Log::LTRACE);
#endif

    LOG_INFO("conducteo is starting.");

    QStringList paths;

    // Ensure plugins are loaded from local path to avoid crash if user
    // has installed another Qt version.
#ifdef WIN32
    QString application_name=QString::fromLatin1(argv[0]);
    QFileInfo application_file(application_name);
    QString path=application_file.path();

    paths<<path;
    QApplication::setLibraryPaths(paths);
#endif

    // Init random generator.
    srand((unsigned int) time(0));

    QApplication app(argc, argv);

    // Icons are not shown into menus.
    app.setAttribute(Qt::AA_DontShowIconsInMenus);
    
    // Set font size under Mac OS X.
#ifdef __APPLE__
    QFont f=QApplication::font();
    f.setPixelSize(11);
    QApplication::setFont(f);
#endif

    // Extentions: copy
    //  * boundaries.db if boundaries-v3.db doesn't exist,
    //  * materials.db if materials-v3.db doesn't exist,
    //  * config.xml if config-v3.xml doesn't exist.
    paths = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
    if (!paths.empty())
    {
        QString qPath = paths.at(0);

        if (!QFile::exists(qPath + "/boundaries-v3.db"))
            QFile::copy(qPath + "/boundaries.db", qPath + "/boundaries-v3.db");

        if (!QFile::exists(qPath + "/materials-v3.db"))
            QFile::copy(qPath + "/materials.db", qPath + "/materials-v3.db");

        if (!QFile::exists(qPath + "/config-v3.xml"))
            QFile::copy(qPath + "/config.xml", qPath + "/config-v3.xml");
    }

    MainWindow *window=createMainWindow();

    // Open file.
    if (argc==2)
        loadMainWindowFile(window, argv[1]);

    int rc=app.exec();

    LOG_INFO("conducteo is shutting down.");

    deleteWindow(window);
    LinguistManager::instance()->kill();
    LocalData::instance()->kill();
    StatesManager::instance()->kill();
    Materials::instance()->kill();
    BoundaryConditions::instance()->kill();
    Environments::instance()->kill();
    ActionsManager::instance()->kill();

#ifdef ENABLE_PROFILER
    Profiler::instance()->writeReport();
    Profiler::instance()->kill();
#endif

    LOG_INFO("Quit main function.");

    return rc;
}
