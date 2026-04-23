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

#ifndef _VOLUME_PROPERTIES_H
#define _VOLUME_PROPERTIES_H

#include <RectangleGeometricalProperties.h>
#include <StatesObserver.h>
#include <MeshProperties.h>
#include <Translatable.h>
#include <QVBoxLayout>
#include <QComboBox>
#include <QWidget>
#include <QLabel>

class VolumeProperties: public QWidget,
                        public Translatable,
                        public StatesObserver
{

    Q_OBJECT

public:

    VolumeProperties(QWidget *parent=0);
    virtual ~VolumeProperties();

    void translate();
    void statesChanged();

protected:

    void paintEvent(QPaintEvent *event);

private slots:

    void dataXChanged(double x);
    void dataYChanged(double y);
    void dataWidthChanged(double width);
    void dataHeightChanged(double height);

private:

    QVBoxLayout                     _layout;
    QHBoxLayout                     _geometryLayout;
    QVBoxLayout                     _labelsLayout;
    QLabel                          _geometryTitle;
    RectangleGeometricalProperties  _geometryData;
    QLabel                          _labelUnit_1;
    QLabel                          _labelUnit_2;
    QLabel                          _labelUnit_3;
    QLabel                          _labelUnit_4;
    QLabel                          _materialTitle;
    QComboBox                       _material;
    QLabel                          _meshTitle;
    MeshProperties                  _meshData;
    
    QString             _title;

};

#endif
