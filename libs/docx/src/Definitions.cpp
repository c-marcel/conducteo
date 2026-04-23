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

#include <Definitions.h>
#include <XmlParser.h>
#include <algorithm>

using namespace Import::Dictionary;

Definitions::Definitions()
{
}

Errors::Code Definitions::load(const std::string &content)
{
    XmlParser parser;
    parser.setXmlStream(content);
    // load parameters.
    std::vector<std::string> parameters = parser.getElementsContentByXPath( "dictionary/parameter" );
    for ( unsigned int i = 0 ; i < parameters.size() ; i ++ )
    {
        std::string parameter = parameters.at( i );
        addParameter( parameter );
    }
    // load arrays.
    std::vector<std::string> arrays = parser.getElementsContentByXPath( "dictionary/array" );
    for ( unsigned int i = 0 ; i < arrays.size() ; i ++ )
    {
        std::string array = arrays.at( i );
        ArrayDefinition arrayDefinition( array );
        _arrays.push_back( arrayDefinition );
    }
    if ( parameters.size() + arrays.size() == 0 )
        return Errors::EmptyDictionary;
    return Errors::NoError;
}

std::vector<std::string> Definitions::availableParameters() const
{
    std::vector<std::string> parameters;
    for ( std::map<std::string, std::string>::const_iterator it = _xpath.begin() ; it != _xpath.end() ; it++ )
        if ( ! it->first.empty() && ! it->second.empty() )
            parameters.push_back( it->first );
    std::sort( parameters.begin(), parameters.end(), & Definitions::stringLengthComparison );
    return parameters;
}

std::string Definitions::getXPath( const std::string & parameter ) const
{
    std::map<std::string, std::string>::const_iterator it = _xpath.find( parameter );
    if ( it == _xpath.end() )
        return "";
    return it->second;
}

void Definitions::addParameter( const std::string & parameter )
{
    XmlParser parser;
    parser.setXmlStream( parameter );
    std::string parameterName  = parser.getXPathValue( "parameter@name"  );
    std::string parameterXPath = parser.getXPathValue( "parameter@xpath" );
    _xpath[ parameterName ] = parameterXPath;
}

std::vector<std::string> Definitions::availableArrays() const
{
    std::vector<std::string> arrays;
    for ( unsigned int i = 0 ; i < _arrays.size() ; i++ )
        arrays.push_back( _arrays.at( i ).getName() );
    std::sort( arrays.begin(), arrays.end(), & Definitions::stringLengthComparison );
    return arrays;
}

ArrayDefinition Definitions::getArrayDefinitionByName( const std::string & name ) const
{
    for ( unsigned int i = 0 ; i < _arrays.size() ; i++ )
        if ( _arrays.at( i ).getName() == name )
            return _arrays.at( i );
    return ArrayDefinition( name );
}

bool Definitions::stringLengthComparison( std::string string1, std::string string2 )
{
    if ( string1.length() > string2.length() )
        return true;
    return false;
}
