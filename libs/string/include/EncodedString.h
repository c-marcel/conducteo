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

#ifndef _ENCODED_STRING_H
#define _ENCODED_STRING_H

#include <string>

class String: public std::string
{

public:

    /**
      * @brief ctor.
      */
    String();

    /**
      * @brief ctor.
      * @param string std::string.
      */
    String( const std::string & string );
    
    /**
      * @brief dtor.
      */
    virtual ~String();

    /**
      * @brief determine if string is pure Asci.
      * @return true if string is pure Asci.
      */
    const bool isAsci() const ;

    /**
      * @brief determine is string is Utf-8.
      * @return true if string is Utf-8.
      */
    const bool isUtf8() const ;

    /**
      * @brief determine is string is latin1.
      * @return true if string is latin1.
      */
    const bool isLatin1() const ;

    /**
      * @brief convert string from latin1 to Utf-8.
      * @return Utf-8 string.
      */
    String latin1ToUtf8() const ;

    /**
      * @brief convert string from Utf-8 to latin1.
      *        Return empty string if not Utf-8.
      * @return latin1 string.
      */
    String utf8ToLatin1() const ;

    /**
      * @brief convert string in Utf-8 anyway.
      * @return Utf-8 String.
      */
    String toUtf8();

    /**
      * @brief convert string in latin1 anyway.
      * @return latin1 String.
      */
    String toLatin1();

    /**
      * @brief remove Utf-8 BOM is present.
      * @param input input string.
      * @return string without Utf-8 BOM.
      */
    static std::string removeUtf8BomIfPresent( const std::string & input );

    /**
      * @brief echape Xml special characters.
      * @return escaped string.
      */
    String escapeXmlSpecialCharacters();

private:

    /**
      * @brief remove Utf-8 BOM is present.
      */
    void removeUtf8BomIfPresent();

};

#endif
