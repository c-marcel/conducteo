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

#ifndef _MATERIAL_PROPERTIES_H
#define _MATERIAL_PROPERTIES_H

#include <Translatable.h>
#include <QTableWidget>

class Material;

class MaterialProperties: public QTableWidget,
                          public Translatable
{
    Q_OBJECT
public:
    MaterialProperties(QWidget *parent=0);
    virtual ~MaterialProperties();

    void translate();

    void setMaterialId(const QString &id);
    void setName(const QString &name);
    void setConductivity(double conductivity);
    void setColor(const QColor &color);
    void setEmissivity(double emissivity);
    void setMu(double mu);
    void setDescription(const QString &description);
    void setIndent(int indent);

private slots:
    void onCellChanged(int row, int column);
    void onCellDoubleClicked(int row, int column);
    void onCurrentSelectedColorChanged(const QColor &);

private:
    Material   *_material;
    bool        _catchUserInputs;
};

#endif
