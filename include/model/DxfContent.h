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

#ifndef _DXF_CONTENT_H
#define _DXF_CONTENT_H

#include <string>

class ErrorHandler;
class DxfData;

class DxfContent
{

public:

    DxfContent();
    DxfContent(const DxfContent &);
    virtual ~DxfContent();
    
    void setDxf(DxfData *dxf);
    void setDxfData(const std::string &data);
    void setSelected(bool selected);
    void setId(const std::string &id);
    
    DxfData *dxf() const;
    bool selected() const;
    std::string id() const;
    std::string data() const;

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

    std::string serialize() const;
    static DxfContent *createFromXml(const std::string &xml, ErrorHandler *error);

private:

    std::string     _id;
    std::string     _data;
    DxfData        *_dxf;
    bool            _selected;

    double          _xMin;
    double          _xMax;
    double          _yMin;
    double          _yMax;
    
};

#endif
