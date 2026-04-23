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

#include <ArrayDefinition.h>
#include <XmlParser.h>

using namespace Import::Dictionary;

ArrayDefinition::ArrayDefinition( const std::string & array )
{
    XmlParser parser;
    parser.setXmlStream( array );
    _name  = parser.getXPathValue( "array@name"  );
    _xpath = parser.getXPathValue( "array@xpath" );
    std::vector<std::string> parameters = parser.getElementsContentByXPath( "array/parameter" );
    for ( unsigned int i = 0 ; i < parameters.size() ; i ++ )
    {
        std::string parameter = parameters.at( i );
        addParameter( parameter );
    }
}

std::string ArrayDefinition::getName() const
{
    return _name;
}

std::string ArrayDefinition::getXPath() const
{
    return _xpath;
}

std::vector<std::string> ArrayDefinition::availableParameters() const
{
    std::vector<std::string> parameters;
    for ( std::map<std::string, std::string>::const_iterator it = _parameters.begin() ; it != _parameters.end() ; it++ )
        if ( ! it->first.empty() && ! it->second.empty() )
            parameters.push_back( it->first );
    return parameters;
}

std::string ArrayDefinition::getParameterXPath( const std::string & parameter ) const
{
    std::map<std::string, std::string>::const_iterator it = _parameters.find( parameter );
    if ( it == _parameters.end() )
        return "";
    return it->second;
}

void ArrayDefinition::addParameter( const std::string & parameter )
{
    XmlParser parser;
    parser.setXmlStream( parameter );
    std::string parameterName  = parser.getXPathValue( "parameter@name"  );
    std::string parameterXPath = parser.getXPathValue( "parameter@xpath" );
    _parameters[ parameterName ] = parameterXPath;
}
