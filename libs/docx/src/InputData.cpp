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

#include <InputData.h>
#include <Definitions.h>
#include <DocxImage.h>
#include <algorithm>
  
using namespace Import::Data;

InputData::InputData()
{
}

InputData::~InputData()
{
    for (unsigned int i=0 ; i<_images.size() ; i++)
        delete _images.at(i);
}

void InputData::addImages(const std::vector<DocxImage*> &images)
{
    _images=images;
}

std::vector<DocxImage*> InputData::getImages() const
{
    return _images;
}

Errors::Code InputData::setDictionary( const std::string & dictionary )
{
    return _dictionary.load( dictionary );
}

InputData::DataType InputData::dataType( const std::string & data ) const
{
    if ( isDataArray( data ) )
        return ArrayData;
    if ( isDataScalar( data ) )
        return ScalarData;
    return UnknownData;
}

std::string InputData::getData( const std::string & name ) const
{
    std::map<std::string, std::string>::const_iterator it = _data.find( name );
    if ( it == _data.end() )
        return "";
    return it->second;
}

std::vector<std::string> InputData::getParameters() const
{
    std::vector<std::string> parameters;
    for ( std::map<std::string, std::string>::const_iterator it = _data.begin() ; it != _data.end() ; it++ )
        parameters.push_back( it->first );
    std::sort( parameters.begin(), parameters.end(), & Dictionary::Definitions::stringLengthComparison );
    return parameters;
}

bool InputData::isDataArray( const std::string & data ) const
{
    for ( unsigned int i = 0 ; i < _arrays.size() ; i++ )
    {
        if ( _arrays.at( i ).getName() == data )
            return true;
    }
    return false;
}

bool InputData::isDataScalar( const std::string & data ) const
{
    std::map<std::string, std::string>::const_iterator it = _data.find( data );
    if ( it == _data.end() )
        return false;
    return true;
}

Array InputData::getArray( const std::string & name ) const
{
    for ( unsigned int i = 0 ; i < _arrays.size() ; i++ )
        if ( _arrays.at( i ).getName() == name )
            return _arrays.at( i );
    return Array( name );
}

std::vector<Array> InputData::getArrays() const
{
    return _arrays;
}
