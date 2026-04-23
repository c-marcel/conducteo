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

#ifndef _XML_PARSER_H
#define _XML_PARSER_H

#include <tinyxml.h>
#include <vector>
#include <string>

#define XmlDomElementHandle unsigned long long

class XmlParser
{
        
public:

    /**
      * @brief ctor.
      */
    XmlParser();

    /**
      * @brief dtor.
      */
    virtual ~XmlParser();

    std::vector<std::string> split(const std::string & text, char separator) const;

    /**
      * @brief set XML file.
      * @param filename XML filename.
      * @return true if file has been loaded.
      */
    bool setXmlFile( const std::string & filename );

    /**
      * @brief set XML stream.
      * @param xml XML stream.
      */
    void setXmlStream( const std::string & xml );

    /**
      * @brief remove namespaces into XML.
      */
    void removeNamespaces();

    /**
      * @brief preserve whitespaces.
      * @param enabled preserve white spaces if set to true.
      */
    static void preserveWhitespaces( bool enabled );

    /**
      * @brief set XML element value or attribute.
      * @param xpath element XPath (e.g.: "Document/Data/ID@root" ).
      * @param value element value or attribute (e.g.: "1234-5678-9012").
      * @return true if updated.
      */
    bool updateElement( const std::string & xpath, const std::string & value );

    /**
      * @brief insert XML part into DOM element.
      * @param xpath element XPath.
      * @param child XML element to include.
      * @return true if added.
      */
    bool insertElement( const std::string & xpath, const XmlParser & child );

    /**
      * @brief get element value or attribute by XPath.
      * @param xpath element XPath (e.g.: "Document/Data/ID@root" ).
      * @return value as string, empty string if XPath is not found.
      */
    std::string getXPathValue( const std::string & xpath );

    /**
      * @brief get XPath DOM elements by name.
      * @param xpath element XPath (e.g.: "Document/Data/Entry" will
      *        return all Entry elements).
      * @return vector of XML streams (one entry by element naming "name").
      */
    std::vector<std::string> getElementsContentByXPath( const std::string & xpath );

    /**
      * @brief get child elements by parent XPath.
      * @param xpath element XPath (e.g.: "Document/Data/Entry" will
      *        return all Data childs elements).
      * @return vector of XML streams.
      */
    std::vector<std::string> getChildsElementsContentByXPath( const std::string & xpath );

    /**
      * @brief get XML stream.
      * @return XML stream.
      */
    std::string getXMLStream() const ;

    /**
      * @brief get all Dom elements handles of type T.
      * @param name Dom element name.
      * @param xpath root node to use. Document root node by default.
      * @return vector of Dom element handles.
      */
    std::vector<XmlDomElementHandle> getAllDomElementHandlesByName( const std::string & name, const std::string & xpath = "" );
    std::vector<XmlDomElementHandle> getAllDomElementHandlesByName( const std::string & name, XmlDomElementHandle parent );

    /**
      * @brief get Dom element content by handle.
      * @param handle element handle.
      * @return element content as string.
      */
    std::string getDomElementHandleContent( XmlDomElementHandle handle );

    /**
      * @brief get sibling element if type is "name".
      * @param handle element handle.
      * @param name element name.
      * @return element handle.
      */
    XmlDomElementHandle getSiblingElement( XmlDomElementHandle handle, const std::string & name );

    /**
      * @brief set XML element value or attribute.
      * @param handle element handle.
      * @param xpath xpath including element name.
      * @param value element value or attribute (e.g.: "1234-5678-9012").
      */
    void updateHandleXPath( XmlDomElementHandle handle, const std::string & xpath, const std::string & value );

    /**
      * @brief get root node name.
      * @return name as string.
      */
    std::string getRootNodeName();

private:

    /**
      * @brief remove namespaces recursively into XML. This function must NOT be
      *        called directly, it is used by removeNamespaces() function.
      * @param node current "root" node.
      */
    void removeNamespacesRecurse( TiXmlNode * node );

    /**
      * @brief remove node namespace.
      * @param node node pointer.
      */
    void removeNodeNamespace( TiXmlNode * node );

    /**
      * @brief get node by XPath. If many nodes have identical XPath return
      *        first entry. Return null pointer if node is not existing.
      * @param xpath node XPath.
      * @return pointer to node or null.
      */
    TiXmlNode * getNodeByXPath( const std::string & xpath );
    
    /**
      * @brief get DOM hierarchy copy of root node.
      * @return pointer to TiXmlNode.
      */
    TiXmlNode * getDOMHierarchy() const ;

    /**
      * @brief get attribute modified by XPath if provided or empty string otherwise.
      * @param xpath element XPath.
      * @return element attribute to modify or empty string if no attribute is provided.
      */
    std::string splitAttribute( const std::string & xpath ) const ;

    /**
      * @brief split XPath into vector of nodes (excluding attribute).
      * @param xpath element XPath.
      * @return vector of nodes.
      */
    std::vector<std::string> splitNodes( const std::string & xpath ) const ;

    /**
      * @brief update DOM element text.
      * @param node DOM element to update.
      * @param value value to set.
      */
    void setNodeText( TiXmlNode * node , const std::string & value );

    /**
      * @brief get node XML stream
      * @param node node to be used.
      * @return XML stream as string.
      */
    std::string getNodeXMLStream( const TiXmlNode * node ) const ;

    /**
      * @brief get Dom elements of type T.
      * @param name Dom element name.
      * @param root root node to use.
      * @return vector of Dom element content (as TiXmlNode).
      */
    std::vector<TiXmlNode*> getDomElementsByName( const std::string & name, TiXmlNode * root ) const ;

    /**
      * @brief get all Dom elements of type T.
      * @param name Dom element name.
      * @param root root node to use.
      * @return vector of Dom element content (as TiXmlNode).
      */
    std::vector<TiXmlNode*> getAllDomElements( TiXmlNode * root ) const ;

    /**
      * @brief get Dom element address by handle.
      * @param handle element handle.
      * @return element address. Null if not found.
      */
    TiXmlNode * getElementByHandle( XmlDomElementHandle handle );

    TiXmlDocument    _xmlDom;
    std::string      _xmlStream;

};

#endif
