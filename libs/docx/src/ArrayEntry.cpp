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

#include <ArrayEntry.h>
#include <EncodedString.h>

using namespace Import::Data;

ArrayEntry::ArrayEntry()
{
}

ArrayEntry::~ArrayEntry()
{
}

void ArrayEntry::add( const std::string & parameter, const std::string & value )
{
    _data[ parameter ] = value;
}

std::string ArrayEntry::getData( const std::string & name ) const
{
    std::map<std::string, std::string>::const_iterator it = _data.find( name );
    if ( it == _data.end() )
        return "";
    return it->second;
}

std::vector<std::string> ArrayEntry::getParameters() const
{
    std::vector<std::string> parameters;
    for ( std::map<std::string, std::string>::const_iterator it = _data.begin() ; it != _data.end() ; it++ )
        parameters.push_back( it->first );
    return parameters;
}

void ArrayEntry::toUtf8()
{
    for ( std::map<std::string, std::string>::iterator it = _data.begin() ; it != _data.end() ; it++ )
        it->second = String( it->second ).toUtf8();
}

void ArrayEntry::escapeXmlSpecialCharacters()
{
    for ( std::map<std::string, std::string>::iterator it = _data.begin() ; it != _data.end() ; it++ )
        it->second = String( it->second ).escapeXmlSpecialCharacters();
}
