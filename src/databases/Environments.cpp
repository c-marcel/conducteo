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

#include <databases/Environments.h>
#include <physics/Environment.h>

Environments::Environments()
{
    // Create environments.
    Environment *env1=new Environment;
    env1->setName("1");
    env1->setColor(204, 12, 12);
    _standard.push_back(env1);

    Environment *env2=new Environment;
    env2->setName("2");
    env2->setColor(111, 242, 111);
    _standard.push_back(env2);

    Environment *env3=new Environment;
    env3->setName("3");
    env3->setColor(0, 136, 255);
    _standard.push_back(env3);

    Environment *env4=new Environment;
    env4->setName("4");
    env4->setColor(231, 237, 40);
    _standard.push_back(env4);

    initializeDescriptions();
}

Environments::~Environments()
{
    for (unsigned int i=0 ; i<_standard.size() ; i++)
        delete _standard.at(i);
}

Environment *Environments::environmentFromName(const std::string &name) const
{
    for (unsigned int i=0 ; i<_standard.size() ; i++)
        if (_standard.at(i)->name()==name)
            return _standard.at(i);
    return 0;
}

void Environments::initializeDescriptions()
{
    Environment *env1 = environmentFromName("1");
    if (env1)
        env1->setDescription("Ambiance thermique 1");

    Environment *env2 = environmentFromName("2");
    if (env2)
        env2->setDescription("Ambiance thermique 2");

    Environment *env3 = environmentFromName("3");
    if (env3)
        env3->setDescription("Ambiance thermique 3");

    Environment *env4 = environmentFromName("4");
    if (env4)
        env4->setDescription("Ambiance thermique 4");
}

void Environments::setDescription(const std::string& name, const std::string& description)
{
    Environment *env = environmentFromName(name);
    if (env)
        env->setDescription(description);
}
