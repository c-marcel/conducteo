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

#include <DocxConverter.h>
#include <DocxImage.h>

using namespace Export::Converter;

DocxConverter::DocxConverter():
_data( 0 )
{
}

DocxConverter::~DocxConverter()
{
}

Errors::Code DocxConverter::load( const std::string & filename )
{
    Errors::Code code = _docxDocument.load( filename );
    // merge DocX Dom elements.
    _docxDocument.mergeDocxElements();
    // formatting replacement expressions.
    _docxDocument.formatReplacementExpression();
    return code;
}

void DocxConverter::setInputData( Import::Data::InputData * data )
{
    _data = data;
}

Errors::Code DocxConverter::exportDocument( const std::string & filename )
{
    convert();
    return _docxDocument.save( filename );
}

void DocxConverter::convert()
{
    if ( ! _data )
        return;

    // retrieve all parameters & update values.
    std::vector<std::string> parameters = _data->getParameters();
    for ( unsigned int i = 0 ; i < parameters.size() ; i++ )
    {
        std::string parameter = parameters.at( i );
        std::string value     = _data->getData( parameter );
        _docxDocument.writeParameterValue( parameter, value );
    }

    // retrieve all array data.
    std::vector<Import::Data::Array> arrays = _data->getArrays();
    for ( unsigned int i = 0 ; i < arrays.size() ; i++ )
        _docxDocument.writeArrayData( arrays.at( i ) );

    // retrieve all images.
    std::vector<DocxImage*> images=_data->getImages();
    for ( unsigned int i = 0 ; i < images.size() ; i++ )
        _docxDocument.writeImage(images.at(i)->id(), images.at(i));
    _docxDocument.ensurePngSupport();
}
