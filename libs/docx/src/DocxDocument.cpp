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

#include <DocxDocument.h>
#include <DocxImage.h>
#include <tools/ToolBox.h>
#include <EncodedString.h>

#define DOCX_XML_DOCUMENT "word/document.xml"

using namespace Export::Format;

DocxDocument::DocxDocument()
{
    // preserve whitespaces for Xml data.
    XmlParser::preserveWhitespaces( true );
}

DocxDocument::~DocxDocument()
{
}

Errors::Code DocxDocument::load( const std::string & filename )
{
    bool loaded = _zipArchive.open( filename );
    if ( ! loaded )
        return Errors::UnableToLoadDocxModel;
    // check for main Xml document is Utf-8 encoded.
    if ( ! isXmlDocumentUtf8Encoded() )
        return Errors::DocxXmlUnsupportedEncoding;
    return Errors::NoError;
}

void DocxDocument::writeParameterValue( const std::string & parameter, const std::string & value )
{
    // Write all parameters into headers & footers & main document.
    // We assume here that headers & footers are stored into
    // documents named footer* and header*

    std::vector<std::string> files=_zipArchive.getFiles();
    for (unsigned int i=0 ; i<files.size() ; i++)
    {
        const std::string &file=files.at(i);
        if (file==DOCX_XML_DOCUMENT || file.find("header")!=std::string::npos || file.find("footer")!=std::string::npos)
        {
            std::string document=_zipArchive.getFileContent(file);
            std::string updated=replaceParameterValue(document, parameter, value);
            _zipArchive.setFileContent(file, updated);
        }
    }
}

void DocxDocument::writeImage(const std::string &parameter, const DocxImage *image)
{
    // Add image into word/media directory.
    std::string image_name="word/media/"+image->id()+".png";
    _zipArchive.setFileContent(image_name, image->data());

    // Add reference into word/_rels/document.xml.rels.
    std::string relations=_zipArchive.getFileContent("word/_rels/document.xml.rels");
    std::string node="<Relationship Id=\"rId"
                     +parameter
                     +"\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/image\" Target=\"media/"
                     +image->id()+".png\" />";
    size_t position=relations.find("</Relationships>");
    if (position)
    {
        std::string out=relations.substr(0, position);
        out+=node;
        out+=relations.substr(position, relations.length()-position);
        _zipArchive.setFileContent("word/_rels/document.xml.rels", out);
    }

    // Replace keyword by image node.
    std::string document=_zipArchive.getFileContent(DOCX_XML_DOCUMENT);
    std::string updated=replaceImageValue(document, parameter, image);
    _zipArchive.setFileContent(DOCX_XML_DOCUMENT, updated);
}

std::string DocxDocument::imageToString(const DocxImage *image, unsigned int size) const
{
	std::string id="rId"+image->id();
	std::string s_width=ToolBox::intToString(image->width()*double(size)*93.6);
	std::string s_height=ToolBox::intToString(image->height()*double(size)*93.6);

	// We must close <w:t> part before & re-open it after.
	std::string line = "</w:t>";
	line += "<w:drawing>";
	line += "<wp:inline>";
	line += "<wp:extent cx=\"" + s_width + "\" cy=\"" + s_height + "\" />";
	line += "<wp:docPr name=\"" + id + "\" id=\"1\" />";
	line += "<a:graphic xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">";
	line += "<a:graphicData uri=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">";
	line += "<pic:pic xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">";
	line += "<pic:nvPicPr>";
	line += "<pic:cNvPr id=\"2\" name=\"" + id + "\" />";
	line += "<pic:cNvPicPr />";
	line += "</pic:nvPicPr>";
	line += "<pic:blipFill>";
	line += "<a:blip r:embed=\"" + id + "\" />";
	line += "<a:stretch>";
	line += "<a:fillRect />";
	line += "</a:stretch>";
	line += "</pic:blipFill>";
	line += "<pic:spPr>";
	line += "<a:xfrm>";
	line += "<a:off x=\"0\" y=\"0\" />";
	line += "<a:ext cx=\"" + s_width + "\" cy=\"" + s_height + "\" />";
	line += "</a:xfrm>";
	line += "<a:prstGeom prst=\"rect\" />";
	line += "</pic:spPr>";
	line += "</pic:pic>";
	line += "</a:graphicData>";
	line += "</a:graphic>";
	line += "</wp:inline>";
	line += "</w:drawing>";
	line += "<w:t>";
	return line;
}

Errors::Code DocxDocument::save( const std::string & filename )
{
    bool saved = _zipArchive.save( filename );
    if ( ! saved )
        return Errors::UnableToSaveDocx;
    return Errors::NoError;
}

std::string DocxDocument::getArrayParameter( const std::string & array, const std::string & parameter ) const
{
    return array + "_" + parameter;
}

std::string DocxDocument::replaceImageValue(const std::string &content, const std::string &parameter, const DocxImage *image) const
{
    std::string replacedString = "${" + parameter + ";";
    std::string output         = content;
    // search for all occurrences.
    size_t position  = output.find( replacedString );
    size_t position2 = output.find( "}", position  );
    while ( position != std::string::npos && position2 != std::string::npos )
    {
        // Read image size.
        unsigned int size=100;
        size_t size_position=output.find("size=", position);
        size_t size_percent=output.find("%", size_position);
        if (size_position!=std::string::npos && size_percent!=std::string::npos && size_percent<position2)
        {
            std::string size_string=output.substr(size_position+5, size_percent-size_position-5);
            std::string size_string_2;
            for (unsigned int i=0 ; i<size_string.size() ; i++)
                if (size_string.at(i)!=' ')
                    size_string_2+=size_string.at(i);
            size=ToolBox::stringToInt(size_string_2);
        }

        // Check for size bounds.
        if (size>200)
            size=200;

        // remove "option}" part.
        size_t position3 = position + replacedString.length();
        output = output.replace( position3, position2 - position3 + 1, "" );

        // Create value.
        std::string value=imageToString(image, size);

        // replace "${keyword;" part.
        output = output.replace( position, replacedString.length(), value );

        // find next occurrence.
        position  = output.find( replacedString );
        position2 = output.find( "}", position  );
    }
    return output;
}

std::string DocxDocument::replaceParameterValue( const std::string & content, const std::string & parameter, const std::string & value ) const
{
    std::string replacedString = "${" + parameter + ";";
    std::string output         = content;
    // search for all occurrences.
    size_t position  = output.find( replacedString );
    size_t position2 = output.find( "}", position  );
    while ( position != std::string::npos && position2 != std::string::npos )
    {
        // remove "option}" part.
        size_t position3 = position + replacedString.length();
        output = output.replace( position3, position2 - position3 + 1, "" );
        // replace "${keyword;" part.
        output = output.replace( position, replacedString.length(), value );
        // find next occurrence.
        position  = output.find( replacedString );
        position2 = output.find( "}", position  );
    }
    return output;
}

std::vector<std::string> DocxDocument::explodeDocumentByLines( const std::string & content, const std::string & array ) const
{
    std::vector<std::string> parts;
    std::vector<size_t>      positions;
    // find all write functions.
    positions.push_back( 0 );
    std::string writeFunction = "${" + array;
    size_t position = content.find( writeFunction );
    while ( position != std::string::npos )
    {
        size_t begin = getBeginOfLinePosition( content, position );
        size_t end   = getEndOfLinePosition( content, position );
        if ( begin != std::string::npos && end != std::string::npos )
        {
            positions.push_back( begin );
            positions.push_back( end );
            position = end;
        }
        position = content.find( writeFunction, position + 1 );
    }
    positions.push_back( content.size() );
    // create parts.
    for ( unsigned int i = 0 ; i < positions.size() - 1 ; i++ )
    {
        size_t      begin = positions.at( i     );
        size_t      end   = positions.at( i + 1 );
        std::string part  = content.substr( begin, end - begin );
        parts.push_back( part );
    }
    return parts;
}

std::string DocxDocument::implodeParts( const std::vector<std::string> & parts ) const
{
    std::string out;
    for ( unsigned int i = 0 ; i < parts.size() ; i++ )
        out += parts.at( i );
    return out;
}

size_t DocxDocument::getBeginOfLinePosition( const std::string & content, size_t position ) const
{
    return content.rfind( "<w:tr ", position );
}

size_t DocxDocument::getEndOfLinePosition( const std::string & content, size_t position ) const
{
    size_t end = content.find( "</w:tr>", position );
    if ( end != std::string::npos )
        end += 7;
    return end;
}

bool DocxDocument::areArrayAndLineTemplateAssociated( const std::string & line, Import::Data::Array & array ) const
{
    std::string writeFunction = "${" + array.getName();
    // search for first write function.
    size_t begin = line.find( writeFunction );
    if ( begin == std::string::npos )
        return false;
    return true;
}

void DocxDocument::writeArrayData( Import::Data::Array & array )
{
    // read word/document.xml.
    std::string filename  = DOCX_XML_DOCUMENT;
    std::string document  = _zipArchive.getFileContent( filename );
    std::string arrayName = array.getName();
    // explode document.
    std::vector<std::string> parts = explodeDocumentByLines( document, arrayName );
    // replace lines.
    for ( unsigned int i = 0 ; i < parts.size() ; i++ )
        if ( areArrayAndLineTemplateAssociated( parts.at( i ), array ) )
            parts.at( i ) = createArray( parts.at( i ), array );
    document = implodeParts( parts );
    _zipArchive.setFileContent( filename, document );
}

std::string DocxDocument::createArray( const std::string & line, Import::Data::Array & array ) const
{
    std::string out;
    for ( unsigned int i = 0 ; i < array.size() ; i++ )
    {
        std::string newLine   = line;
        std::string arrayName = array.getName();
        std::vector<std::string> parameters = array.at( i ).getParameters();
        bool validLine = true;
        bool emptyLine = true;
        for ( unsigned int j = 0 ; j < parameters.size() ; j++ )
        {
            std::string parameter = parameters.at( j );
            std::string value     = array.at( i ).getData( parameter );
            std::string keyword   = getArrayParameter( arrayName, parameter );
            // check for keyword is not mandatory if no value is provided.
            if ( value.empty() && isKeywordMandatory( newLine, keyword ) )
                validLine = false;
            if ( ! value.empty() )
                emptyLine = false;
            newLine = replaceParameterValue( newLine, keyword, value );
        }
        // if line is empty: hide left & right cells borders.
        if ( emptyLine )
            newLine = hideLeftAndRightCellsBorders( newLine );
        if ( validLine )
            out += newLine;
    }
    return out;
}

std::string DocxDocument::hideLeftAndRightCellsBorders( std::string line ) const
{
    // look for all <w:left w:val="single" & replace with <w:left w:val="nil".
    size_t position = line.find( "<w:left w:val=\"single\"" );
    while ( position != std::string::npos )
    {
        line.replace( position, 22, "<w:left w:val=\"nil\"" );
        position = line.find( "<w:left w:val=\"single\"" );
    }

    // look for all <w:right w:val="single" & replace with <w:right w:val="nil".
    position = line.find( "<w:right w:val=\"single\"" );
    while ( position != std::string::npos )
    {
        line.replace( position, 23, "<w:right w:val=\"nil\"" );
        position = line.find( "<w:right w:val=\"single\"" );
    }
    return line;
}

const bool DocxDocument::isXmlDocumentUtf8Encoded() const
{
    std::string filename = DOCX_XML_DOCUMENT;
    std::string document = _zipArchive.getFileContent( filename );

    // remove Utf-8 BOM if present.
    document = String::removeUtf8BomIfPresent( document );

    // try removing name spaces.
    XmlParser parser;
    parser.setXmlStream( document );
    parser.removeNamespaces();
    return ! parser.getXMLStream().empty();
}

void DocxDocument::mergeDocxElements()
{
    // read word/document.xml.
    std::string filename  = DOCX_XML_DOCUMENT;
    std::string document  = _zipArchive.getFileContent( filename );
    // get all <r> Dom elements.
    XmlParser parser;
    parser.setXmlStream( document );
    std::vector<XmlDomElementHandle> elements = parser.getAllDomElementHandlesByName( "w:r" );
    if ( elements.empty() )
        return;
    // loop for on all elements.
    for ( unsigned int i = 0 ; i < elements.size() - 1 ; i++ )
    {
        // look for next <r> element.
        XmlDomElementHandle   element        = elements.at( i );
        std::string           elementContent = parser.getDomElementHandleContent( element );
        XmlParser elementParser;
        elementParser.setXmlStream( elementContent );
        std::string elementText = elementParser.getXPathValue( "w:r/w:t" );
        size_t      position    = elementText.find( "${" );
        if ( position != std::string::npos )
        {
            XmlDomElementHandle next = getSiblingDomElement( parser, element );
            bool nextElementEnabled = true;
            while ( next && nextElementEnabled )
            {
                // read contents & texts.
                std::string nextContent = parser.getDomElementHandleContent( next );
                XmlParser nextParser;
                nextParser.setXmlStream( nextContent );
                std::string nextText = nextParser.getXPathValue( "w:r/w:t" );
                // check for end element.
                std::string part = nextText;
                std::string rest;
                if ( nextText.find( "}" ) != std::string::npos )
                {
                    nextElementEnabled = false;
                    part = nextText.substr( 0, nextText.find( "}" ) + 1 );
                    rest = nextText.substr( nextText.find( "}" ) + 1, nextText.length() - nextText.find( "}" ) - 1 );
                }
                // create new texts.
                elementText += part;
                // update next element.
                parser.updateHandleXPath( next, "w:r/w:t", rest );
                // get next element.
                next = getSiblingDomElement( parser, next );
            }
        }
        // update element.
        parser.updateHandleXPath( element, "w:r/w:t", elementText );
    }
    document = parser.getXMLStream();
    _zipArchive.setFileContent( filename, document );
}

void DocxDocument::formatReplacementExpression()
{
    std::vector<std::string> files=_zipArchive.getFiles();
    for (unsigned int i=0 ; i<files.size() ; i++)
    {
        const std::string &file=files.at(i);
        if (file==DOCX_XML_DOCUMENT || file.find("header")!=std::string::npos || file.find("footer")!=std::string::npos)
        {
            formatReplacementExpression(file);
        }
    }
}

void DocxDocument::formatReplacementExpression(const std::string &filename)
{
    // read word/document.xml.
    std::string document  = _zipArchive.getFileContent( filename );
    // get all <r> Dom elements.
    XmlParser parser;
    parser.setXmlStream( document );
    std::vector<XmlDomElementHandle> elements = parser.getAllDomElementHandlesByName( "w:r" );
    if ( elements.empty() )
        return;
    // loop for on all elements.
    for ( unsigned int i = 0 ; i < elements.size() ; i++ )
    {
        XmlDomElementHandle   element        = elements.at( i );
        std::string           elementContent = parser.getDomElementHandleContent( element );
        XmlParser elementParser;
        elementParser.setXmlStream( elementContent );
        std::string elementText = elementParser.getXPathValue( "w:r/w:t" );
        size_t            begin = elementText.find( "${"       );
        size_t              end = elementText.find( "}", begin );
        while ( begin != std::string::npos && end != std::string::npos )
        {
            std::string expression    = elementText.substr( begin, end - begin + 1 );
            std::string formattedExpr = formatExpression( expression );
            elementText.replace( begin, end - begin + 1, formattedExpr );
            begin = elementText.find( "${", end );
            end   = elementText.find( "}", begin );
        }
        parser.updateHandleXPath( element, "w:r/w:t", elementText );
    }
    document = parser.getXMLStream();
    _zipArchive.setFileContent( filename, document );
}

XmlDomElementHandle DocxDocument::getSiblingDomElement( XmlParser & parser, XmlDomElementHandle element ) const
{
    XmlDomElementHandle next = parser.getSiblingElement( element, "w:r" );
    if ( next )
        return next;
    XmlDomElementHandle element1 = parser.getSiblingElement( element, "w:proofErr"      );
    XmlDomElementHandle element2 = parser.getSiblingElement( element, "w:bookmarkStart" );
    XmlDomElementHandle element3 = parser.getSiblingElement( element, "w:bookmarkEnd"   );
    if ( ! element1 && ! element2 && ! element3 )
        return 0;
    if ( element1 )
        element = element1;
    else if ( element2 )
        element = element2;
    else if ( element3 )
        element = element3;
    return getSiblingDomElement( parser, element );
}

std::string DocxDocument::formatExpression( const std::string & input ) const
{
    // input string "${___keyword___option___}". Where _ is one or more white spaces.
    std::string output;
    bool        keywordFound  = false;
    bool        optionSepUsed = false;
    for ( unsigned int i = 0 ; i < input.size() ; i++ )
    {
        // add option separator.
        if (    keywordFound
             && ! optionSepUsed
             && ( input.at( i ) == ' ' || input.at( i ) == '}' ) )
        {
            output        += ';';
            optionSepUsed  = true;
        }
        if ( input.at( i ) != ' ' )
            output += input.at( i );
        // keyword found.
        if ( input.at( i ) != ' ' && input.at( i ) != '$' && input.at( i ) != '{' )
            keywordFound = true;
    }
    return output;
}

bool DocxDocument::isKeywordMandatory( const std::string & content, const std::string & parameter ) const
{
    std::string replacedString = "${" + parameter + ";";
    // search for all occurrences.
    size_t position  = content.find( replacedString );
    size_t position2 = content.find( "}", position  );
    while ( position != std::string::npos && position2 != std::string::npos )
    {
        std::string data = content.substr( position, position2 - position + 1 );
        if ( data.find( ";mandatory" ) != std::string::npos )
            return true;
        // find next occurrence.
        position  = content.find( replacedString, position2 );
        position2 = content.find( "}",            position  );
    }
    return false;
}

void DocxDocument::ensurePngSupport()
{
    // Ensure <Default Extension="png" ContentType="image/png"/> to be present.
    std::string data=_zipArchive.getFileContent("[Content_Types].xml");
    size_t position=data.find("image/png");
    
    // Already present.
    if (position!=std::string::npos)
        return;

    // Add node before </Types>.
    position=data.find("</Types>");
    if (position==std::string::npos)
        return;

    std::string out=data.substr(0, position);
    out+="<Default Extension=\"png\" ContentType=\"image/png\"/>";
    out+=data.substr(position, data.length()-position);
    
    _zipArchive.setFileContent("[Content_Types].xml", out);
}
