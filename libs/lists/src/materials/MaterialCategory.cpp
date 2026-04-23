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

#include <materials/MaterialCategory.h>
#include <tinyxml.h>

MaterialCategory::MaterialCategory(QTreeWidgetItem *parent):
    QTreeWidgetItem(parent)
{
    setSizeHint(0, QSize(200, 25));
}

MaterialCategory::~MaterialCategory()
{
}

void MaterialCategory::setId(const QString &id)
{
    _id=id;
}

void MaterialCategory::setParentId(const QString &id)
{
    _parentId=id;
}

void MaterialCategory::setTitle(const QString &title)
{
    setText(0, title);
    _title=title;
}

QString MaterialCategory::id() const
{
    return _id;
}

QString MaterialCategory::parentId() const
{
    return _parentId;
}

QString MaterialCategory::title() const
{
    return _title;
}

std::string MaterialCategory::serialize() const
{
    std::string xml="<category id=\"";
    xml+=_id.toStdString();
    xml+="\" name=\"";
    xml+=_title.toStdString();
    xml+="\"";
    
    MaterialCategory *p=dynamic_cast<MaterialCategory*>(parent());
    if (p)
    {
        xml+=" parent=\"";
        xml+=p->id().toStdString();
        xml+="\"";
    }

    xml+=" />";
    return xml;
}

MaterialCategory *MaterialCategory::createFromXml(const std::string &xml)
{
    TiXmlDocument doc;
    doc.Parse(xml.c_str());

    TiXmlElement *category=doc.RootElement();
    if (!category)
        return 0;

    std::string root_name=category->Value();
    if (root_name!="category")
        return 0;

    if (!category->Attribute("id"))
        return 0;
    if (!category->Attribute("name"))
        return 0;

    QString id=category->Attribute("id");
    QString name=category->Attribute("name");
    QString parentId;
    if (category->Attribute("parent"))
        parentId=category->Attribute("parent");

    MaterialCategory *c=new MaterialCategory;
    c->setId(id);
    c->setParentId(parentId);
    c->setTitle(name);
    return c;
}
