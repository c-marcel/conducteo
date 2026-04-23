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

#include <geometry/Path.h>
#include <errors/ErrorHandler.h>
#include <geometry/Line.h>
#include <geometry/Arc.h>
#include <tinyxml.h>

Path::Path()
{
}

Path::~Path()
{
}

bool Path::check(ErrorHandler *error)
{
    // No check.
    return true;
}

bool Path::isVertical() const
{
    return false;
}

Path *Path::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *root=doc.RootElement();
    if (!root)
    {
        setStaticError(error, BadPathNode);
        return 0;
    }

    if (!root->Value())
    {
        setStaticError(error, BadPathNode);
        return 0;
    }

    std::string type=root->Value();
    if (type=="line")
        return Line::createFromXml(xml, error);
    if (type=="arc")
        return Arc::createFromXml(xml, error);
    setStaticError(error, UnknownPathType);
    return 0;
}

double Path::angle() const
{
    return 0.0;
}
