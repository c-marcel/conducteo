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

#include <XmlParser.h>
#include <tools/ToolBox.h>
#include <sstream>
  
XmlParser::XmlParser()
{
}

XmlParser::~XmlParser()
{
}

std::vector<std::string> XmlParser::split( const std::string & text, char separator ) const
{
    std::vector<std::string> list;
    std::stringstream ss( text );
    std::string item;
    while( std::getline( ss, item, separator ) )
        list.push_back( item );
    return list;
}

bool XmlParser::setXmlFile( const std::string & filename )
{
    _xmlDom = TiXmlDocument( filename.c_str() );
    return _xmlDom.LoadFile();
}

void XmlParser::setXmlStream( const std::string & xml )
{
    _xmlDom.Parse( ( const char * ) xml.c_str(), 0, TIXML_ENCODING_LEGACY );
}

bool XmlParser::updateElement( const std::string & xpath, const std::string & value )
{
    // retrieve element or attribute.
    TiXmlNode * node = getNodeByXPath( xpath );
    if ( ! node )
        return false;
    // retrieve attribute.
    std::string attribute = splitAttribute( xpath );
    // set element value or attribute value.
    if ( attribute.empty() )
    {
        setNodeText( node, value );
    }
    else
    {
        if ( ! node->ToElement() )
            return false;
        node->ToElement()->SetAttribute( attribute.c_str(), value.c_str() );
    }
    return true;
}

bool XmlParser::insertElement( const std::string & xpath, const XmlParser & child )
{
    // retrieve parent element.
    TiXmlNode * parent = getNodeByXPath( xpath );
    if ( ! parent )
        return false;
    // get child DOM sub-hierarchy.
    TiXmlNode * sub = child.getDOMHierarchy();
    if ( ! sub )
        return false;
    // insert child.
    parent->InsertEndChild( * sub );
    return true;
}

std::string XmlParser::getXPathValue( const std::string & xpath )
{
    // retrieve element or attribute.
    TiXmlNode * node = getNodeByXPath( xpath );
    if ( ! node )
        return "";
    // retrieve attribute.
    std::string attribute = splitAttribute( xpath );
    // get element value or attribute value.
    if ( attribute.empty() )
    {
        if ( node->ToElement() && node->ToElement()->GetText() )
            return node->ToElement()->GetText();
    }
    else
    {
        if ( node->ToElement() && node->ToElement()->Attribute( attribute.c_str() ) )
            return node->ToElement()->Attribute( attribute.c_str() );
    }
    return "";
}

std::vector<std::string> XmlParser::getElementsContentByXPath( const std::string & xpath )
{
    std::vector<std::string> elements;
    // retrieve first node defined by XPath.
    TiXmlNode * node = getNodeByXPath( xpath );
    // retrieve node name.
    std::string s_name = "";
    if ( node && node->Value() )
        s_name = node->Value();
    while ( node )
    {
        // store current node.
        std::string content = getNodeXMLStream( node );
        elements.push_back( content );
        node = node->NextSibling( s_name.c_str() );
    }
    return elements;
}

std::vector<std::string> XmlParser::getChildsElementsContentByXPath( const std::string & xpath )
{
    std::vector<std::string> elements;
    // retrieve node defined by XPath.
    TiXmlNode * node = getNodeByXPath( xpath );
    if ( ! node )
        return elements;
    node = node->FirstChild();
    while ( node )
    {
        // store current node.
        std::string content = getNodeXMLStream( node );
        elements.push_back( content );
        node = node->NextSibling();
    }
    return elements;
}

std::string XmlParser::getXMLStream() const
{
    // retrieve XML stream.
    const TiXmlElement * root = _xmlDom.RootElement();
    if ( ! root ) 
        return "";
    return getNodeXMLStream( root );
}

std::vector<XmlDomElementHandle> XmlParser::getAllDomElementHandlesByName( const std::string & name, const std::string & xpath )
{
    TiXmlNode * root = getNodeByXPath( xpath );
    if ( ! root )
        root = _xmlDom.RootElement();
    std::vector<TiXmlNode*>  nodes = getDomElementsByName( name, root );
    std::vector<XmlDomElementHandle> s_nodes;
    for ( unsigned int i = 0 ; i < nodes.size() ; i++ )
    {
        TiXmlNode * node = nodes.at( i );
        s_nodes.push_back( ( XmlDomElementHandle ) node );
    }
    return s_nodes;
}

std::vector<XmlDomElementHandle> XmlParser::getAllDomElementHandlesByName( const std::string & name, XmlDomElementHandle handle )
{
    // retrieve node.
    std::vector<XmlDomElementHandle> s_nodes;
    TiXmlNode * root = getElementByHandle( handle );
    if ( ! root )
        return s_nodes;
    std::vector<TiXmlNode*>  nodes = getDomElementsByName( name, root );
    for ( unsigned int i = 0 ; i < nodes.size() ; i++ )
    {
        TiXmlNode * node = nodes.at( i );
        s_nodes.push_back( ( XmlDomElementHandle ) node );
    }
    return s_nodes;
}

std::string XmlParser::getDomElementHandleContent( XmlDomElementHandle handle )
{
    TiXmlNode * node = getElementByHandle( handle );
    if ( ! node )
        return "";
    return getNodeXMLStream( node );
}

XmlDomElementHandle XmlParser::getSiblingElement( XmlDomElementHandle handle, const std::string & name )
{
    TiXmlNode * node = getElementByHandle( handle );
    if ( ! node )
        return 0;
    TiXmlNode * next = node->NextSibling();
    if ( ! next )
        return 0;
    if ( next->Value() == name )
        return ( XmlDomElementHandle ) next;
    return 0;
}

void XmlParser::updateHandleXPath( XmlDomElementHandle handle, const std::string & xpath, const std::string & value )
{
    // retrieve node.
    TiXmlNode * node = getElementByHandle( handle );
    if ( ! node )
        return;
    // retrieve node list.
    std::vector<std::string> nodes = splitNodes( xpath );
    if ( nodes.empty() )
        return;
    // progress into Dom hierarchy.
    for ( unsigned int i = 1 ; i < nodes.size() ; i++ )
    {
        if ( ! node )
            return;
        node = node->FirstChild( nodes.at( i ).c_str() );
    }
    if ( ! node )
        return;
    // retrieve attribute.
    std::string attribute = splitAttribute( xpath );
    // set element value or attribute value.
    if ( attribute.empty() )
    {
        setNodeText( node, value );
    }
    else
    {
        if ( ! node->ToElement() )
            return;
        node->ToElement()->SetAttribute( attribute.c_str(), value.c_str() );
    }
}

std::string XmlParser::getRootNodeName()
{
    if ( ! _xmlDom.RootElement() )
        return "";
    if ( ! _xmlDom.RootElement()->Value() )
        return "";
    return _xmlDom.RootElement()->Value();
}

TiXmlNode * XmlParser::getDOMHierarchy() const 
{
    if ( ! _xmlDom.RootElement() )
        return 0;
    return _xmlDom.RootElement()->Clone();
}

void XmlParser::removeNamespaces()
{
    // retrieve root node.
    TiXmlNode * root = _xmlDom.RootElement();
    if ( ! root ) 
        return;
    // recursive calls.
    return removeNamespacesRecurse( root );
}

void XmlParser::preserveWhitespaces( bool enabled )
{
    TiXmlBase::SetCondenseWhiteSpace( ! enabled );
}

void XmlParser::removeNamespacesRecurse( TiXmlNode * root )
{
    if ( ! root )
        return;
    TiXmlNode * node = root;
    // remove all sibling nodes namespaces & call recursively
    // this function to apply with children too.
    while( node )
    {
        // remove node namespace.
        removeNodeNamespace( node );
        // recursive call.
        TiXmlNode * c_node = node->FirstChild();
        removeNamespacesRecurse( c_node );
        // next child.
        node = node->NextSibling();
    }
}

void XmlParser::removeNodeNamespace( TiXmlNode * node )
{
    if ( ! node )
        return;
    TiXmlElement * e_node = node->ToElement();
    if ( ! e_node )
        return;
    const char * c_name = e_node->Value();
    if ( ! c_name )
        return;
    std::string s_name = c_name;
    size_t ns_end = s_name.find( ":" );
    if ( ns_end == std::string::npos )
        return;
    std::string s_name2 = s_name.substr( ns_end + 1, s_name.length() - ns_end - 1 );
    e_node->SetValue( s_name2.c_str() );
}

TiXmlNode * XmlParser::getNodeByXPath( const std::string & xpath )
{
    // retrieve node list.
    std::vector<std::string> nodes = splitNodes( xpath );
    // progress into DOM hierarchy.
    if ( nodes.empty() )
        return 0;
    TiXmlNode * node = _xmlDom.FirstChild( nodes.at( 0 ).c_str() );
    for ( unsigned int i = 1 ; i < nodes.size() ; i++ )
    {
        if ( ! node )
            return 0;
        node = node->FirstChild( nodes.at( i ).c_str() );
    }
    return node;
}

std::string XmlParser::splitAttribute( const std::string & xpath ) const
{
    // assuming that XPath is written as node1/node2/node3@attribute
    // attribute is defined as end of string after @ if existing.
    size_t at_position = xpath.find( "@" );
    if ( at_position == std::string::npos )
        return "";
    return xpath.substr( at_position + 1, xpath.length() - at_position - 1 );
}

std::vector<std::string> XmlParser::splitNodes( const std::string & xpath ) const
{
    std::string xpath2 = xpath;
    // remove attribute part.
    if ( xpath.find( "@" ) != std::string::npos )
        xpath2 = xpath.substr( 0, xpath.find( "@" ) );
    return split( xpath2, '/' );
}

void XmlParser::setNodeText( TiXmlNode * node , const std::string & value )
{
    TiXmlNode * child = node->FirstChild();
    while ( child )
    {
        TiXmlText * text = child->ToText();
        if ( text && text->Value() )
        {
            node->RemoveChild( text );
            break;
        }
        child = child->NextSibling();
    }
    child = node->FirstChild();
    TiXmlText * text = new TiXmlText( value.c_str() );
    if ( child )
        node->InsertBeforeChild( child , * text );
    else
        node->LinkEndChild( text ) ;
}

std::string XmlParser::getNodeXMLStream( const TiXmlNode * node ) const
{
    if ( ! node )
        return "";
    // dump text content.
    TiXmlPrinter printer;
    node->Accept( & printer );
    const char * xmlcstr = printer.CStr();
    if ( ! xmlcstr )
        return "";
    return xmlcstr;
}

std::vector<TiXmlNode*> XmlParser::getDomElementsByName( const std::string & name, TiXmlNode * root ) const
{
    std::vector<TiXmlNode*> all = getAllDomElements( root );
    std::vector<TiXmlNode*> nodes;
    for ( unsigned int i = 0 ; i < all.size() ; i++ )
    {
        TiXmlNode * node = all.at( i );
        if ( node->Value() && node->Value() == name )
            nodes.push_back( node );
    }
    return nodes;
}

std::vector<TiXmlNode*> XmlParser::getAllDomElements( TiXmlNode * root ) const
{
    std::vector<TiXmlNode*> nodes;
    if ( ! root )
        return nodes;
    TiXmlNode * node = root->FirstChild();
    while( node )
    {
        nodes.push_back( node );
        // recursive call.
        std::vector<TiXmlNode*> childrens = getAllDomElements( node );
        for ( unsigned int i = 0 ; i < childrens.size() ; i++ )
            nodes.push_back( childrens.at( i ) );
        // next child.
        node = node->NextSibling();
    }
    return nodes;
}

TiXmlNode * XmlParser::getElementByHandle( XmlDomElementHandle handle )
{
    // return pointer by handle without checks.
    return ( TiXmlNode * ) handle;
    // return pointer by handle with checks.
    TiXmlNode * root = _xmlDom.RootElement();
    std::vector<TiXmlNode*> nodes = getAllDomElements( root );
    for ( unsigned int i = 0 ; i < nodes.size() ; i++ )
    {
        if ( ( XmlDomElementHandle ) nodes.at( i ) == handle )
        {
            return nodes.at( i );
        }
    }
    return 0;
}
