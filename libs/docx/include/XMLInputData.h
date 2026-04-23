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

#ifndef _XML_INPUT_DATA_H
#define _XML_INPUT_DATA_H

#include <InputData.h>
#include <vector>

namespace Import
{

    namespace Data
    {

        class XMLInputData: public InputData
        {

        public:

            /**
              * @brief ctor.
              */
            XMLInputData();
            
            /**
              * @brief dtor.
              */
            virtual ~XMLInputData();
            
            /**
              * @brief parse file to retrieve all data.
              * @param filename filename to parse.
              * @return error code.
              */
            Errors::Code load( const std::string & filename );

        private:

            /**
              * @brief load parameters for provided XML file.
              * @param filename XML filename.
              * @param parameters vector of parameters.
              * @return error code.
              */
            Errors::Code loadParametersFromXML( const std::string & filename, const std::vector<std::string> & parameters );

            /**
              * @brief load arrays for provided XML file.
              * @param filename XML filename.
              * @param arrays vector of arrays.
              * @return error code.
              */
            Errors::Code loadArraysFromXML( const std::string & filename, const std::vector<std::string> & arrays );

            /**
              * @brief load array by array definition.
              * @filename input filename.
              * @param array array definition.
              * @return array.
              */
            Array loadArrayByDefinition( const std::string & filename, Import::Dictionary::ArrayDefinition array ) const ;

        };
        
    }

}

#endif
