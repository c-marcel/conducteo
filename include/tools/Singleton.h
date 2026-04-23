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

#ifndef _SINGLETON_H
#define _SINGLETON_H

template <typename T>
class Singleton
{

protected:

    Singleton()
    {
    }
    
    virtual ~Singleton()
    {
    }

public:

    static T *instance()
    {
        if(_instance==0)
            _instance=new T;
        return (static_cast<T*>(_instance));
    }

    static void kill()
    {
        if(_instance!=0)
        {
            delete _instance;
            _instance=0;
        }
    }

private:

    static T *_instance;

};

template <typename T>
T* Singleton<T>::_instance=0;

#endif
