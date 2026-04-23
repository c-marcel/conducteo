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

#ifndef _MESH_PROPERTIES_H
#define _MESH_PROPERTIES_H

#include <StatesObserver.h>
#include <Translatable.h>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <vector>

class Volume;

class MeshProperties: public QWidget,
                      public Translatable,
                      public StatesObserver
{

    Q_OBJECT

public:

    MeshProperties(QWidget *parent=0);
    virtual ~MeshProperties();

    void translate();
    void statesChanged();

private slots:

    void radioButton1Toggled(bool checked);
    void radioButton2Toggled(bool checked);
    void radioButton3Toggled(bool checked);
    void editingFinished();

private:

    QVBoxLayout             _layout;
    QHBoxLayout             _meshSizeLayout1;
    QHBoxLayout             _meshSizeLayout2;
    QHBoxLayout             _meshSizeLayout3;
    QRadioButton            _radio_1;
    QRadioButton            _radio_2;
    QRadioButton            _radio_3;
    QLineEdit               _meshSizeInput;
    QLabel                  _meshSizeLabel;
    bool                    _catchUserInputs;
    std::vector<Volume*>    _volumes;

};

#endif
