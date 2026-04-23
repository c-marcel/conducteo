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

#include <model/DxfContent.h>
#include <errors/ErrorHandler.h>
#include <tools/ToolBox.h>
#include <model/DxfData.h>
#include <tinyxml.h>

DxfContent::DxfContent():
    _dxf(0),
    _selected(false),
    _id(ToolBox::getNewUuuid())
{
}

DxfContent::DxfContent(const DxfContent &dxf)
{
    _selected=dxf._selected;
    _data=dxf._data;
    _id=dxf._id;
    _dxf=0;
}

void DxfContent::setId(const std::string &id)
{
    _id=id;
}

std::string DxfContent::id() const
{
    return _id;
}

DxfContent::~DxfContent()
{
    if (_dxf)
        delete _dxf;
}

void DxfContent::setDxf(DxfData *dxf)
{
    _dxf=dxf;
    _xMin=_dxf->xMin();
    _xMax=_dxf->xMax();
    _yMin=_dxf->yMin();
    _yMax=_dxf->yMax();
}

void DxfContent::setDxfData(const std::string &data)
{
    _data=data;
}

DxfData *DxfContent::dxf() const
{
    return _dxf;
}

std::string DxfContent::data() const
{
    return _data;
}

DxfContent *DxfContent::createFromXml(const std::string &xml, ErrorHandler *error)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *node=doc.RootElement();
    if (!node)
    {
        setStaticError(error, BadDxfImportNode);
        return 0;
    }

    if (!node->Value())
    {
        setStaticError(error, BadDxfImportNode);
        return 0;
    }

    std::string root_name=node->Value();
    if (root_name!="dxf-import")
    {
        setStaticError(error, BadDxfImportNode);
        return 0;
    }

    DxfContent *dxf=new DxfContent;

    // Data.
    TiXmlElement *data=node->FirstChildElement("data");
    if (data && data->GetText())
        dxf->setDxfData(data->GetText());

    return dxf;
}

std::string DxfContent::serialize() const
{
    std::string xml;
    xml+="<dxf-import>";
    xml+="<data>"+_data+"</data>";
    xml+="</dxf-import>";
    return xml;
}

void DxfContent::setSelected(bool selected)
{
    _selected=selected;
}

bool DxfContent::selected() const
{
    return _selected;
}

double DxfContent::xMin() const
{
    return _xMin;
}

double DxfContent::xMax() const
{
    return _xMax;
}

double DxfContent::yMin() const
{
    return _yMin;
}

double DxfContent::yMax() const
{
    return _yMax;
}
