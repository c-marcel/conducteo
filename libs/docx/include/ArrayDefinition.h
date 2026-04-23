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

#ifndef _ARRAY_DEFINITION_H
#define _ARRAY_DEFINITION_H

#include <string>
#include <vector>
#include <map>

namespace Import
{

    namespace Dictionary
    {

        class ArrayDefinition
        {

        public:

            /**
              * @brief ctor.
              * @param array array as XML DOM element.
              * @param xpath array xpath.
              */
            ArrayDefinition( const std::string & array );
            
            /**
              * @brief get array name.
              * @return array name as string.
              */
            std::string getName() const ;
            
            /**
              * @brief get array XPath.
              * @return array XPath as string.
              */
            std::string getXPath() const ;

            /**
              * @brief get all available & non empty parameters.
              * @return vector of parameters as strings.
              */
            std::vector<std::string> availableParameters() const ;
            
            /**
              * @brief get XPath for provided scalar parameter.
              * @param parameter parameter name.
              * @return parameter XPath as string.
              */
            std::string getParameterXPath( const std::string & parameter ) const ;
            
        private:

            /**
              * @brief add parameter.
              * @param parameter parameter as XML DOM element.
              */
            void addParameter( const std::string & parameter );

            std::map<std::string, std::string>  _parameters;
            std::string                         _name;
            std::string                         _xpath;

        };
        
    }

}

#endif
