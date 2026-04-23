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

#ifndef _TOOL_BOX_H
#define _TOOL_BOX_H

#include <numeric/EnvironmentResult.h>
#include <algorithm>
#include <string>
#include <vector>

class TiXmlElement;

class ToolBox
{

public:

    static std::vector<std::string> split(const std::string &s, char delim);
    static std::string merge(const std::vector<std::string> &list, const char separator);
    static double stringToDouble(const std::string &value);
    static double convertStringToDouble(const std::string &value);
    static int stringToInt(const std::string &value);
    static std::string intToString(int value);
    static std::string doubleToString(double value);
    static std::string doubleToString(double value, int precision);
    static std::string domToString(TiXmlElement *element);
    static std::string getNewUuuid();
    static std::string readFile(const std::string &filename);
    static std::vector<std::string> invert(const std::vector<std::string> &in);
    static std::string escapeXmlCharacters(const std::string &in);

    static void setDecimalSeparator(char separator);
    static std::string useDecimalSeparator(const std::string& input);

    static double globalCouplingCoefficient(const std::vector<EnvironmentResult>& envs, const std::string& id);
    static bool useEnvironment(const std::vector<EnvironmentResult>& envs, const std::string& id);

    template <class T>
    static void uniquify(std::vector<T>& v)
    {
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
    }

};

#endif
