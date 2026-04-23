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

#ifndef _INPUT_DATA_H
#define _INPUT_DATA_H

#include <Definitions.h>
#include <Array.h>

class DocxImage;

namespace Import
{

    namespace Data
    {

        class InputData
        {

        public:

            enum DataType
            {
                UnknownData,
                ScalarData,
                ArrayData
            };

            /**
              * @brief ctor.
              */
            InputData();
            
            /**
              * @brief dtor.
              */
            virtual ~InputData();
            
            /**
              * @brief set dictionary.
              * @param dictionary dictionary filename.
              * @return error code.
              */
            Errors::Code setDictionary( const std::string & dictionary );
            
            /**
              * @brief parse file to retrieve all data.
              * @param filename filename to parse.
              * @return error code.
              */
            virtual Errors::Code load( const std::string & filename ) = 0 ;

            /**
              * @brief add some images to Docx.
              * @param images Vector of images.
              */
            void addImages(const std::vector<DocxImage*> &images);

            /**
              * @brief get data type.
              * @param data data name.
              * @return data type.
              */
            DataType dataType( const std::string & data ) const ;
            
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
              * @brief get all arrays.
              * @return vector of arrays.
              */
            std::vector<Array> getArrays() const ;

            /**
              * @brief get all images.
              * @return vector of images.
              */
            std::vector<DocxImage*> getImages() const;

            /**
              * @brief get array data.
              * @param name array name.
              * @return copy of array.
              */
            Array getArray( const std::string & name ) const ;
            
        protected:
        
            Import::Dictionary::Definitions         _dictionary;
            std::map<std::string, std::string>      _data;
            std::vector<Array>                      _arrays;
            std::vector<DocxImage*>                 _images;

        private:

            /**
              * @brief check if data is an array.
              * @param name data name.
              * @return true if data is an array.
              */
            bool isDataArray( const std::string & data ) const ;

            /**
              * @brief check if data is a scalar.
              * @param data data name.
              * @return true if data is scalar.
              */
            bool isDataScalar( const std::string & data ) const ;

        };
        
    }

}

#endif
