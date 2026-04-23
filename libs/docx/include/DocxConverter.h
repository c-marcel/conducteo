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

#ifndef _DOCX_CONVERTER_H
#define _DOCX_CONVERTER_H

#include <DocxDocument.h>
#include <InputData.h>

namespace Export
{

    namespace Converter
    {

        class DocxConverter
        {

        public:

            /**
              * @brief ctor.
              */
            DocxConverter();
            
            /**
              * @brief dtor.
              */
            virtual ~DocxConverter();
            
            /**
              * @brief load Docx document (template).
              * @param filename Docx filename.
              * @return error code.
              */
            Errors::Code load( const std::string & filename );
            
            /**
              * @brief write parameter value.
              * @param data input data.
              */
            void setInputData( Import::Data::InputData * data );
            
            /**
              * @brief save Docx document.
              * @param filename Docx document filename.
              * @return error code.
              */
            Errors::Code exportDocument( const std::string & filename );

        private:

            /**
              * @brief convert Docx template.
              */
            void convert();

            Format::DocxDocument      _docxDocument;
            Import::Data::InputData * _data;

        };
        
    }

}

#endif
