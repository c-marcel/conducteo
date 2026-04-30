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

#ifndef _LINGUIST_MANAGER_H
#define _LINGUIST_MANAGER_H

#include <tools/Singleton.h>
#include <QString>
#include <vector>
#include <map>

class Translatable;

class LinguistManager: public Singleton<LinguistManager>
{

public:

    enum Language
    {
        French,
        English
    };

    LinguistManager();
    virtual ~LinguistManager();

    void registerObject(Translatable *object);
    void unregisterObject(Translatable *object);
    
    void changeLanguage(Language language);
    QString translate(const QString &text) const;

    Language currentLanguage() const;
    std::string languageToString() const;

private:
    void loadTranslations(const QString& language);

    std::vector<Translatable*>  _objects;
    std::map<QString, QString>  _translations;
    Language                    _currentLanguage;

};

#endif
