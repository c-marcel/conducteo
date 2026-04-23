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

#include <EncodedString.h>

String::String():
std::string( "" )
{
}

String::String( const std::string & string ):
std::string( string )
{
    removeUtf8BomIfPresent();
}

String::~String()
{
}

const bool String::isAsci() const
{
    for ( unsigned int i = 0 ; i < size() ; i++ )
    {
        unsigned char value = at( i );
        if ( value & 0x80 )
            return false;
    }
    return true;
}

String String::latin1ToUtf8() const
{
    String output;
    for ( unsigned int i = 0 ; i < size() ; i++ )
    {
        unsigned char value = at( i );
        // non-asci char.
        if ( value & 0x80 )
        {
            output += 0xc0 | ( value >> 6 );
            output += 0x80 | ( value & 0x3f );
        }
        // asci char.
        else
        {
            output += value;
        }
    }
    return output;
}

String String::utf8ToLatin1() const
{
    String output;
    for ( unsigned int i = 0 ; i < size() ; i++ )
    {
        unsigned char value = at( i );
        // non-asci char.
        unsigned char toto = value & 0xc0;
        unsigned char titi = 0xc0;
        if ( ( value & 0xc0 ) == 0xc0 )
        {
            // check for Utf-8.
            if ( i + 1 == size() )
                return std::string("");
            unsigned char value1 = at( i + 1 );
            // check for Utf-8.
            if ( ( value1 & 0x80 ) != 0x80 )
                return std::string("");
            unsigned char first = value & 0x03;
            first = first << 6;
            unsigned char second = value1 & 0x3f;
            unsigned char third = first | second;
            output += third;
        }
        // asci char.
        else if ( ! ( value & 0x80 ) )
        {
            output += value;
        }
    }
    return output;
}

const bool String::isUtf8() const
{
    // perform operations:
    // 1. utf-8 to latin-1,
    // 2. latin-1 to utf-8.
    // if string was utf-8, resultant string will be equal to input string.
    // if string was latin-1, strings will differ.
    String latin1 = utf8ToLatin1();
    String utf8   = latin1.latin1ToUtf8();
    return ( *this == utf8 );
}

const bool String::isLatin1() const
{
    // see isUtf8 method.
    String utf8   = latin1ToUtf8();
    String latin1 = utf8.utf8ToLatin1();
    return ( *this == latin1 );
}

String String::toUtf8()
{
    if ( isUtf8() )
        return *this;
    if ( isLatin1() )
        return latin1ToUtf8();
    return *this;
}

String String::toLatin1()
{
    if ( isUtf8() )
        return utf8ToLatin1();
    if ( isLatin1() )
        return *this;
    return *this;
}

void String::removeUtf8BomIfPresent()
{
    // remove Utf-8 BOM is present.
    if ( length() < 3 )
        return;
    unsigned char first  = at( 0 );
    unsigned char second = at( 1 );
    unsigned char third  = at( 2 );
    if ( first != 0xef )
        return;
    if ( second != 0xbb )
        return;
    if ( third != 0xbf )
        return;
    // Utf-8 BOM is present: remove it.
    std::string withoutBomPart = substr( 3, length() - 3 );
    clear();
    insert( 0, withoutBomPart );
}

std::string String::removeUtf8BomIfPresent( const std::string & input )
{
    if ( input.length() >= 3 )
    {
        unsigned char first  = input.at( 0 );
        unsigned char second = input.at( 1 );
        unsigned char third  = input.at( 2 );
        if ( first == 0xef && second == 0xbb && third == 0xbf )
            return input.substr( 3, input.length() - 3 );
    }
    return input;
}

String String::escapeXmlSpecialCharacters()
{
    std::string output;
    for ( unsigned int i = 0 ; i < length() ; i++ )
    {
        if ( at( i ) == '&' )
            output += "&amp;";
        else if ( at( i ) == '<' )
            output += "&lt;";
        else if ( at( i ) == '>' )
            output += "&gt;";
        else if ( at( i ) == '"' )
            output += "&quot;";
        else if ( at( i ) == '\'' )
            output += "&apos;";
        else
            output += at( i );
    }
    return output;
}
