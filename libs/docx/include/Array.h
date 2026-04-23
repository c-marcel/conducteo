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

#ifndef _ARRAY_H
#define _ARRAY_H

#include <ArrayEntry.h>
#include <DocxErrors.h>

namespace Import
{

    namespace Data
    {

        class Array: public std::vector<ArrayEntry>
        {

        public:

            /**
              * @brief ctor.
              * @param name array name
              */
            Array( const std::string & name );
            
            /**
              * @brief dtor.
              */
            virtual ~Array();
            
            /**
              * @brief get array name.
              * @return array name as string.
              */
            std::string getName() const ;

            /**
              * @brief set error.
              * @param error error.
              */
            void setError( Errors::Code error );

            /**
              * @brief get error.
              * @return error.
              */
            Errors::Code getError() const ;

            /**
              * @brief ensure Utf-8.
              */
            void toUtf8();

            /**
              * @brief escape Xml special characters.
              */
            void escapeXmlSpecialCharacters();
            
        private:
        
            std::string     _name;
            Errors::Code    _error;

        };
        
    }

}

#endif
