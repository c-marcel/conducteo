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

#include <Array.h>

using namespace Import::Data;

Array::Array( const std::string & name ):
_name( name ),
_error( Errors::NoError )
{
}

Array::~Array()
{
}

std::string Array::getName() const
{
    return _name;
}

void Array::setError( Errors::Code error )
{
    _error = error;
}

Errors::Code Array::getError() const
{
    return _error;
}

void Import::Data::Array::toUtf8()
{
    for ( unsigned int i = 0 ; i < size() ; i++ )
        at( i ).toUtf8();
}

void Import::Data::Array::escapeXmlSpecialCharacters()
{
    for ( unsigned int i = 0 ; i < size() ; i++ )
        at( i ).escapeXmlSpecialCharacters();
}
