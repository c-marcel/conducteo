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

#ifndef _DOCX_DOCUMENT_H
#define _DOCX_DOCUMENT_H

#include <tools/ZipArchive.h>
#include <DocxErrors.h>
#include <XmlParser.h>
#include <InputData.h>
#include <Array.h>

namespace Export
{

    namespace Format
    {

        class DocxDocument
        {

        public:

            /**
              * @brief ctor.
              */
            DocxDocument();
            
            /**
              * @brief dtor.
              */
            virtual ~DocxDocument();
            
            /**
              * @brief load Docx document (template).
              * @param filename Docx filename.
              * @return error code.
              */
            Errors::Code load( const std::string & filename );
            
            /**
              * @brief write parameter value.
              * @param parameter parameter name.
              * @param value parameter value.
              */
            void writeParameterValue( const std::string & parameter, const std::string & value );

            /**
              * @brief write array data.
              * @param array array reference.
              */
            void writeArrayData( Import::Data::Array & array );

            /**
              * @brief write image.
              * @param parameter image parameter name.
              * @param image image data.
              */
            void writeImage(const std::string &parameter, const DocxImage *image);
            
            /**
              * @brief save Docx document.
              * @param filename Docx document filename.
              * @return error code.
              */
            Errors::Code save( const std::string & filename );

            /**
              * @brief parse Docx document to merge Xml Dom elements splitted
              *        as $ and data where value must be stored as $data. Imported
              *        Docx document remains unmodified on disk.
              */
            void mergeDocxElements();

            /**
              * @brief format ${...} expression as ${keyword;option} by removing
              *        all white spaces & replacing white spaces between keyword
              *        & option by semicolon.
              */
            void formatReplacementExpression();

            /**
              * @brief Write Xls charts with input data.
              * @param path Temporary path.
              * @param data Input data.
              */
            void writeXlsCharts( const std::string & path, Import::Data::InputData * data );

            /**
              * @brief Ensure Png support.
              */
            void ensurePngSupport();

        private:

            /**
              * @brief create parameter function for array.
              * @param array array name.
              * @param parameter parameter name.
              * @return parameter function.
              */
            std::string getArrayParameter( const std::string & array, const std::string & parameter ) const ;

            /**
              * @brief replace parameter by value.
              * @param content string to be filled.
              * @param parameter parameter name.
              * @param value parameter value.
              * @return filled string.
              */
            std::string replaceParameterValue( const std::string & content, const std::string & parameter, const std::string & value ) const ;
            std::string replaceImageValue(const std::string &content, const std::string &parameter, const DocxImage *image) const;

            /**
              * @brief split document into parts as "raw" parts & "line" parts.
              * @param content document content.
              * @param array array name to search for.
              * @return vector of parts as strings.
              */
            std::vector<std::string> explodeDocumentByLines( const std::string & content, const std::string & array ) const ;

            /**
              * @brief implode parts to single document.
              * @param parts vector of strings.
              * @return single document.
              */
            std::string implodeParts( const std::vector<std::string> & parts ) const ;

            /**
              * @brief get begin of line position.
              * @param content document content.
              * @param position inner position.
              * @return begin of line.
              */
            size_t getBeginOfLinePosition( const std::string & content, size_t position ) const ;

            /**
              * @brief get end of line position.
              * @param content document content.
              * @param position inner position.
              * @return begin of line.
              */
            size_t getEndOfLinePosition( const std::string & content, size_t position ) const ;

            /**
              * @brief determine if line template is associated with provided array.
              * @param line line template as string.
              * @param array array reference.
              * @return true if template & array are associated.
              */
            bool areArrayAndLineTemplateAssociated( const std::string & line, Import::Data::Array & array ) const ;

            /**
              * @brief create array content with array data & line template.
              * @param line line template.
              * @param array array reference.
              * @return array content.
              */
            std::string createArray( const std::string & line, Import::Data::Array & array ) const ;

            /**
              * @brief hide left & right cells borders.
              * @param line Line entry.
              * @return line without left & right cells borders.
              */
            std::string hideLeftAndRightCellsBorders( std::string line ) const ;

            /**
              * @brief check for Docx main Xml document is Utf-8 encoded.
              * @return true if encoding is Utf-8.
              */
            const bool isXmlDocumentUtf8Encoded() const ;

            /**
              * @brief get next sibling element of type "w:r" or "w:proofErr".
              * @param parser Xml parser.
              * @param element element handle.
              * @return handle.
              */
            XmlDomElementHandle getSiblingDomElement( XmlParser & parser, XmlDomElementHandle element ) const ;

            /**
              * @brief format replacement expression.
              * @param input input string.
              * @return formatted string.
              */
            std::string formatExpression( const std::string & input ) const ;

            /**
              * @brief determine if keyword is mandatory.
              * @param content Content as string.
              * @param parameter Parameter name.
              * @return True if parameter is mandatory.
              */
            bool isKeywordMandatory( const std::string & content, const std::string & parameter ) const ;

            /**
              * @brief format ${...} expression as ${keyword;option} by removing
              *        all white spaces & replacing white spaces between keyword
              *        & option by semicolon.
              * param filename Filename to be processed.
              */
            void formatReplacementExpression(const std::string &filename);

            std::string imageToString(const DocxImage *image, unsigned int size) const;

            ZipArchive    _zipArchive;

        };
        
    }

}

#endif
