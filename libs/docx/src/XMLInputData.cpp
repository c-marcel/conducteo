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

#include <XMLInputData.h>
#include <EncodedString.h>
#include <XmlParser.h>
  
using namespace Import::Data;

XMLInputData::XMLInputData()
{
}

XMLInputData::~XMLInputData()
{
}

Errors::Code XMLInputData::load( const std::string & filename )
{
    // load parameters.
    std::vector<std::string> parameters = _dictionary.availableParameters();
    Errors::Code code = loadParametersFromXML( filename, parameters );
    if ( code == Errors::UnableToParseXmlData )
        return code;
    // load arrays.
    std::vector<std::string> arrays = _dictionary.availableArrays();
    Errors::Code code2 = loadArraysFromXML( filename, arrays );
    // check for content.
    if ( code == Errors::NoXmlData && code2 == Errors::NoXmlData )
        return Errors::NoXmlData;
    if ( code2 != Errors::NoError && code2 != Errors::NoXmlData )
        return code2;
    return Errors::NoError;
}

Errors::Code XMLInputData::loadParametersFromXML( const std::string & filename, const std::vector<std::string> & parameters )
{
    XmlParser parser;
    parser.setXmlStream( filename );
    unsigned int undefinedVariableFound = 0;
    for ( unsigned int i = 0 ; i < parameters.size() ; i++ )
    {
        std::string parameterName  = parameters.at( i );
        std::string parameterXPath = _dictionary.getXPath( parameterName );
        std::string parameterValue = parser.getXPathValue( parameterXPath );
        if ( parameterXPath.empty() )
            undefinedVariableFound += 1;
        // ensure Utf-8 conversion.
        parameterValue = String( parameterValue ).toUtf8().escapeXmlSpecialCharacters();
        _data[ parameterName ] = parameterValue;
    }
    if ( undefinedVariableFound == parameters.size() )
        return Errors::NoXmlData;
    if ( undefinedVariableFound > 0 )
        return Errors::UndefinedVariablesFound;
    return Errors::NoError;
}

Errors::Code XMLInputData::loadArraysFromXML( const std::string & filename, const std::vector<std::string> & arrays )
{
    XmlParser parser;
    parser.setXmlStream( filename );
    unsigned int undefinedVariablesFound = 0;
    unsigned int noXmlData               = 0;
    for ( unsigned int i = 0 ; i < arrays.size() ; i++ )
    {
        std::string                         arrayName       = arrays.at( i );
        Import::Dictionary::ArrayDefinition arrayDefinition = _dictionary.getArrayDefinitionByName( arrayName );
        // create Array & read all values.
        Array array = loadArrayByDefinition( filename, arrayDefinition );
        if ( array.getError() == Errors::UndefinedVariablesFound )
            undefinedVariablesFound += 1;
        else if ( array.getError() == Errors::NoXmlData )
            noXmlData += 1;
        _arrays.push_back( array );
    }
    if ( noXmlData == arrays.size() )
        return Errors::NoXmlData;
    if ( undefinedVariablesFound > 0 )
        return Errors::UndefinedVariablesFound;
    return Errors::NoError;
}

Array XMLInputData::loadArrayByDefinition( const std::string & filename, Import::Dictionary::ArrayDefinition array ) const
{
    Array data( array.getName() );
    // retrieve array data.
    XmlParser parser;
    parser.setXmlStream( filename );
    std::vector<std::string> entries    = parser.getElementsContentByXPath( array.getXPath() );
    std::vector<std::string> parameters = array.availableParameters();
    unsigned int noXmlData = 0;
    for ( unsigned int i = 0 ; i < entries.size() ; i++ )
    {
        ArrayEntry            entry;
        XmlParser entryParser;
        entryParser.setXmlStream( entries.at( i ) );
        unsigned int undefinedVariable = 0;
        for ( unsigned int j = 0 ; j < parameters.size() ; j++ )
        {
            std::string parameterName  = parameters.at( j );
            std::string parameterXpath = array.getParameterXPath( parameterName );
            std::string parameterValue = entryParser.getXPathValue( parameterXpath );
            if ( parameterXpath.empty() )
            {
                undefinedVariable += 1;
                data.setError( Errors::UndefinedVariablesFound );
            }
            // ensure Utf-8 conversion.
            parameterValue = String( parameterValue ).toUtf8().escapeXmlSpecialCharacters();
            entry.add( parameterName, parameterValue );
        }
        if ( undefinedVariable == parameters.size() )
            noXmlData += 1;
        data.push_back( entry );
    }
    if ( noXmlData == entries.size() )
        data.setError( Errors::NoXmlData );
    return data;
}
