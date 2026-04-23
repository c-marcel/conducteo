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

#ifndef _ARRAY_ENTRY_H
#define _ARRAY_ENTRY_H

#include <vector>
#include <string>
#include <map>

namespace Import
{

    namespace Data
    {

        class ArrayEntry
        {

        public:

            /**
              * @brief ctor.
              */
            ArrayEntry();
            
            /**
              * @brief dtor.
              */
            virtual ~ArrayEntry();

            /**
              * @brief add parameter & value.
              * @param parameter parameter name.
              * @param value parameter value.
              */
            void add( const std::string & parameter, const std::string & value );
            
            /**
              * @brief get data for provided name.
              * @param name data name.
              * @return data as string.
              */
            std::string getData( const std::string & name ) const ;

            /**
              * @brief get all parameters.
              * @return vector of parameters.
              */
            std::vector<std::string> getParameters() const ;

            /**
              * @brief ensure Utf-8.
              */
            void toUtf8();

            /**
              * @brief escape Xml special characters.
              */
            void escapeXmlSpecialCharacters();

        protected:
        
            std::map<std::string, std::string>  _data;

        };
        
    }

}

#endif
