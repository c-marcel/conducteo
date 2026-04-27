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

#include <tools/UiTools.h>
#include <LinguistManager.h>
#include <QApplication>
#include <QDateTime>

QString UiTools::getSoftwareVersion()
{
    QString major=QString::number(CONDUCTEO_VERSION_MAJOR);
    QString minor=QString::number(CONDUCTEO_VERSION_MINOR);
    QString patch=QString::number(CONDUCTEO_VERSION_PATCH);

    return major+"."+minor+"."+patch;
}

QString UiTools::getCurrentDate()
{
    QDateTime date_time=QDateTime::currentDateTime();
    QDate date=date_time.date();
    QString s_date=QString::number(date.day());
    s_date+=" ";
    if (date.month()==1)
        s_date+="janvier";
    else if (date.month()==2)
        s_date+="février";
    else if (date.month()==3)
        s_date+="mars";
    else if (date.month()==4)
        s_date+="avril";
    else if (date.month()==5)
        s_date+="mai";
    else if (date.month()==6)
        s_date+="juin";
    else if (date.month()==7)
        s_date+="juillet";
    else if (date.month()==8)
        s_date+="août";
    else if (date.month()==9)
        s_date+="septembre";
    else if (date.month()==10)
        s_date+="octobre";
    else if (date.month()==11)
        s_date+="novembre";
    else if (date.month()==12)
        s_date+="décembre";
    s_date+=" ";
    s_date+=QString::number(date.year());

    s_date+=" ";
    s_date+=LinguistManager::instance()->translate("à");
    s_date+=" ";

    QTime t=date_time.time();
    s_date+=QString::number(t.hour());
    s_date+="h";
    QString m=QString::number(t.minute());
    if (m.size()==1)
        m="0"+m;
    s_date+=m;
    return s_date;
}

QString UiTools::getBinaryDir()
{
#ifdef WIN32
    return QApplication::applicationDirPath();
#else
    return "/usr/bin";
#endif
}

QString UiTools::getDataDir()
{
#ifdef WIN32
    return QApplication::applicationDirPath();
#else
    return "/usr/share/conducteo/data";
#endif
}

QString UiTools::getDocumentationDir()
{
#ifdef WIN32
    return QApplication::applicationDirPath() + "/documentation";
#else
    return "/usr/share/conducteo/documentation";
#endif
}

QString UiTools::getExamplesDir()
{
#ifdef WIN32
    return QApplication::applicationDirPath() + "/examples";
#else
    return "/usr/share/conducteo/examples";
#endif
}

QString UiTools::getTemplatesDir()
{
#ifdef WIN32
    return QApplication::applicationDirPath() + "/templates";
#else
    return "/usr/share/conducteo/templates";
#endif
}

