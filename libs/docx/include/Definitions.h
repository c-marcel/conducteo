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

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#include <ArrayDefinition.h>
#include <DocxErrors.h>

namespace Import
{

    namespace Dictionary
    {

        class Definitions
        {

        public:

            /**
              * @brief ctor.
              */
            Definitions();
            
            /**
              * @brief parse file to retrieve all data.
              * @param filename Definitions filename to parse.
              * @return error code.
              */
            Errors::Code load( const std::string & filename );

            /**
              * @brief get all available & non empty parameters.
              * @return vector of parameters as strings.
              */
            std::vector<std::string> availableParameters() const ;

            /**
              * @brief get all available arrays.
              * @return vector of array names as strings.
              */
            std::vector<std::string> availableArrays() const ;
            
            /**
              * @brief get XPath for provided scalar parameter.
              * @param parameter parameter name.
              * @return parameter XPath as string.
              */
            std::string getXPath( const std::string & parameter ) const ;

            /**
              * @brief get array by name.
              * @param name array name.
              * @return array.
              */
            ArrayDefinition getArrayDefinitionByName( const std::string & name ) const ;

            /**
              * @brief comparison function to sort parameters.
              * @param string1 first string.
              * @param string2 second string.
              */
            static bool stringLengthComparison( std::string string1, std::string string2 );
            
        private:

            /**
              * @brief add parameter.
              * @param parameter parameter as XML DOM element.
              */
            void addParameter( const std::string & parameter );
        
            std::map<std::string, std::string>  _xpath;
            std::vector<ArrayDefinition>        _arrays;

        };
        
    }

}

#endif
