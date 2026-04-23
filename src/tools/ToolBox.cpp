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

#include <tools/ToolBox.h>
#include <tinyxml.h>
#include <sstream>
#include <fstream>
#include <cmath>

#if defined WIN32
    #include <windows.h>
#else
    #include <uuid/uuid.h>
#endif

static char s_decimal_separator = ',';

std::vector<std::string> ToolBox::split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while(std::getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

std::string ToolBox::merge(const std::vector<std::string> &list, const char separator)
{
    std::string out;
    for (unsigned int i=0 ; i<list.size() ; i++)
    {
        out+=list.at(i);
        if (i<list.size()-1)
            out+=separator;
    }
    return out;
}

double ToolBox::stringToDouble(const std::string &value)
{
    std::istringstream buffer(value);
    double d_value;
    buffer >> d_value;
    return d_value;
}

double ToolBox::convertStringToDouble(const std::string &value)
{
    std::string value_s = value;
    size_t position = value_s.find(",");
    if (position != std::string::npos )
        value_s.at(position ) = '.';
    return stringToDouble(value_s);
}

int ToolBox::stringToInt(const std::string &value)
{
    std::istringstream buffer(value);
    int d_value = 0;
    buffer >> d_value;
    return d_value;
}

std::string ToolBox::intToString(int value)
{
    std::ostringstream convert;
    convert << value;
    return convert.str();
}

std::string ToolBox::doubleToString(double value)
{
    std::ostringstream convert;
    convert.precision(18);
    convert << value;
    return convert.str();
}

std::string ToolBox::doubleToString(double value, int precision)
{
    std::string int_part = doubleToString(int(value));

    if (precision == 0)
        return int_part;

    double dec = (value - int(value)) * pow(10.0, precision);
    std::string dec_part = doubleToString(int(dec));

    while (dec_part.length() < precision)
        dec_part = "0" + dec_part;

    return int_part + "." + dec_part;
}

std::string ToolBox::getNewUuuid()
{
#ifdef WIN32
    std::wstring ret = L"";
    GUID sessionGUID = GUID_NULL;
    HRESULT hr = CoCreateGuid( &sessionGUID );
    if( SUCCEEDED( hr ) )
    {
        WCHAR szGUID[40];
        if( StringFromGUID2( sessionGUID, szGUID, 40 ) > 0 )
            ret = szGUID;
    }
    std::string temp( ret.length(), ' ' );
    std::copy( ret.begin(), ret.end(), temp.begin() );
    return temp;
#else
    uuid_t      _UUID;
    char        _UUIDChar[128];
    std::string _UUIDStr;
    uuid_generate( _UUID );
    uuid_unparse ( _UUID, &_UUIDChar[0] );
    _UUIDStr = _UUIDChar;
    return _UUIDStr;
#endif
}

std::string ToolBox::domToString(TiXmlElement *element)
{
    TiXmlPrinter printer;
    element->Accept(&printer);
    const char *s=printer.CStr();
    if (!s)
        return "";
    return s;
}

std::string ToolBox::readFile(const std::string &filename)
{
    std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open())
        return "";
    int size=file.tellg();
    char *buffer = (char*) malloc(sizeof(char)*(size));
    memset(buffer, 0, size);
    file.seekg(0, std::ios::beg);
    if(!file.read(buffer, size))
        return "";
    std::string output;
    for (int i=0 ; i<size ; i++)
        output+=buffer[i];
    free(buffer);
    file.close();
    return output;
}

std::vector<std::string> ToolBox::invert(const std::vector<std::string> &in)
{
    std::vector<std::string> out;
    for (unsigned int i=0 ; i<in.size() ; i++)
        out.insert(out.begin(), in.at(i));
    return out;
}

std::string ToolBox::escapeXmlCharacters(const std::string &in)
{
    std::string output;
    for (unsigned int i=0 ; i<in.length() ; i++)
    {
        if (in.at(i)=='&')
            output+="&amp;";
        else if (in.at(i)=='<')
            output+="&lt;";
        else if (in.at(i)=='>')
            output+="&gt;";
        else if (in.at(i)=='"')
            output+="&quot;";
        else if (in.at(i)=='\'')
            output+="&apos;";
        else
            output+=in.at(i);
    }
    return output;
}

void ToolBox::setDecimalSeparator(char separator)
{
    s_decimal_separator = separator;
}

std::string ToolBox::useDecimalSeparator(const std::string &input)
{
    std::string out = input;

    for (unsigned int i=0 ; i<out.size() ; i++)
        if (out[i] == '.')
            out[i] = s_decimal_separator;

    return out;
}

double ToolBox::globalCouplingCoefficient(const std::vector<EnvironmentResult>& envs, const std::string &id)
{
    double value = 0.0;

    for (unsigned int i=0 ; i<envs.size() ; i++)
    {
        const EnvironmentResult& result = envs.at(i);

        if (result._environment1Id == id)
            value += result._couplingCoefficient;

        if (result._environment2Id == id)
            value += result._couplingCoefficient;
    }

    return value;
}

bool ToolBox::useEnvironment(const std::vector<EnvironmentResult>& envs, const std::string &id)
{
    for (unsigned int i=0 ; i<envs.size() ; i++)
    {
        const EnvironmentResult& result = envs.at(i);

        if (result._environment1Id == id)
            return true;

        if (result._environment2Id == id)
            return true;
    }

    return false;
}
