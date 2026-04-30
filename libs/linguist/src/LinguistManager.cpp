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

#include <LinguistManager.h>
#include <nlohmann/json.hpp>
#include <tools/ToolBox.h>
#include <tools/UiTools.h>
#include <Translatable.h>
#include <fstream>
#include <Log.h>

LinguistManager::LinguistManager()
{
#ifdef __linux__
    changeLanguage(English);    //< English as default language for Linux.
#else
    changeLanguage(French);
#endif
}

LinguistManager::~LinguistManager()
{
}

void LinguistManager::registerObject(Translatable *object)
{
    _objects.push_back(object);
}

void LinguistManager::unregisterObject(Translatable *object)
{
    for (unsigned int i=0 ; i<_objects.size() ; i++)
    {
        if (_objects.at(i)==object)
        {
            _objects.erase(_objects.begin()+i);
            return;
        }
    }
}

LinguistManager::Language LinguistManager::currentLanguage() const
{
    return _currentLanguage;
}

void LinguistManager::loadTranslations(const QString& language)
{
#ifdef WIN32
    std::string filename = UiTools::getTranslationsDir().toLatin1().constData();
#else
    std::string filename = UiTools::getTranslationsDir().toStdString();
#endif

    filename += "/" + language.toStdString() + ".json";

    std::ifstream file(filename);

    if (!file.is_open())
    {
        LOG_ERROR("Error while loading translation file: " + filename);
        return;
    }

    nlohmann::json j;

    try
    {
        file >> j;
    }

    catch (const nlohmann::json::parse_error& e)
    {
        LOG_ERROR("Error while parsing translation file: " + filename);
        LOG_ERROR("Json error: " + e.what());
        return;
    }

    if (!j.is_object())
    {
        LOG_ERROR("JSON root is not an object in file: " + filename);
        return;
    }

    _translations.clear();

    for (auto& [key, value] : j.items())
    {
        try
        {
            if (!value.is_string())
            {
                LOG_WARNING("Value for key '" + key + "' is not a string. Ignored.");
                continue;
            }

            _translations[QString::fromStdString(key)] = QString::fromStdString(value.get<std::string>());
        }

        catch (const std::exception& e)
        {
            LOG_WARNING("Error while reading key '" + key + "'. Details: " + e.what());
        }
    }
}

std::string LinguistManager::languageToString() const
{
    if (_currentLanguage == English)
        return "en";

    return "fr";
}

void LinguistManager::changeLanguage(Language language)
{
    _currentLanguage = language;

    // Load translations.
    std::string l = languageToString();
    loadTranslations(l.c_str());

    // Retranslate ui.
    for (unsigned int i=0 ; i<_objects.size() ; i++)
        _objects.at(i)->translate();

    // Update decimal separator.
    if (language == French)
        ToolBox::setDecimalSeparator(',');
    else
        ToolBox::setDecimalSeparator('.');
}

QString LinguistManager::translate(const QString &text) const
{
    auto it = _translations.find(text);
    if (it != _translations.end())
        return it->second;

    return text;
}
