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

#include <LocalData.h>
#include <QStandardPaths>
#include <tinyxml.h>
#include <QFile>
#include <Log.h>
#include <QDir>

LocalData::LocalData():
    _autosave(false)
{
    createLocalPath();
    enableAutoSave();
    load();
}

LocalData::~LocalData()
{
}

void LocalData::createLocalPath()
{
    // Get local path.
    QStringList paths=QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
    if (paths.empty())
        return;

    // Save path.
    QString qPath=paths.at(0);
    _path=qPath.toStdString();

    // Create path if needed.
    QDir dir;
    dir.mkpath(qPath);
}

void LocalData::setParameter(const std::string &parameter, const std::string &value)
{
    _parameters[parameter]=value;
    if (_autosave)
        save();
}

std::string LocalData::getParameter(const std::string &parameter)
{
    return _parameters[parameter];
}

void LocalData::save()
{
    LOG_INFO("Save local data.");

    std::string out="<?xml version=\"1.0\"?>";
    out+="<parameters>";
    for (std::map<std::string, std::string>::iterator it=_parameters.begin() ; it!=_parameters.end() ; it++)
        out+="<parameter name=\""+it->first+"\">"+it->second+"</parameter>";
    out+="</parameters>";
    
    QFile file(QString(_path.c_str()) + "/config-v3.xml");
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(out.c_str());
    file.close();
}

void LocalData::load()
{
    LOG_INFO("Load local data.");

    _parameters.clear();

    QFile file(QString(_path.c_str()) + "/config-v3.xml");

    // On load if file doesn't exist set show-quickstart-tutorial to true.
    if (!file.exists())
    {
        setParameter("show-quickstart-tutorial", "true");
        save();
    }

    if (!file.open(QIODevice::ReadOnly))
        return;

    QString xml=file.readAll();
    file.close();

    TiXmlDocument doc;
    doc.Parse(xml.toStdString().c_str());

    TiXmlNode *parameters=doc.FirstChildElement("parameters");
    if (!parameters)
        return;

    TiXmlNode *parameter=parameters->FirstChildElement("parameter");
    while (parameter)
    {
        std::string s_name;
        std::string s_value;

        if (parameter->ToElement() && parameter->ToElement()->Attribute("name"))
            s_name=parameter->ToElement()->Attribute("name");

        if (parameter->ToElement() && parameter->ToElement()->GetText())
            s_value=parameter->ToElement()->GetText();

        if (!s_name.empty())
            _parameters[s_name]=s_value;

        parameter=parameter->NextSibling();
    }
}

void LocalData::enableAutoSave(const bool autosave)
{
    _autosave=autosave;
}

std::string LocalData::getLocalPath() const
{
    return _path;
}
