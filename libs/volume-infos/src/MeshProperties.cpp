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

#include <MeshProperties.h>
#include <actions/model/SetVolumesMeshSize.h>
#include <actions/ActionsManager.h>
#include <tools/ToolBox.h>
#include <StatesManager.h>
#include <model/Volume.h>
#include <QHeaderView>

MeshProperties::MeshProperties(QWidget *parent):
    QWidget(parent),
    _layout(this),
    _catchUserInputs(true)
{
	
#ifdef __APPLE__
	int s=25;
#else
	int s=15;
#endif

#ifdef __APPLE__
    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(6);
#endif
	
    _radio_1.setFixedHeight(s);
    _meshSizeLayout1.addWidget(&_radio_1);
    _layout.addLayout(&_meshSizeLayout1);
	
    _radio_2.setFixedHeight(s);
    _meshSizeLayout2.addWidget(&_radio_2);
    _layout.addLayout(&_meshSizeLayout2);

    _radio_3.setFixedHeight(s);
    _meshSizeInput.setFixedWidth(50);
    _meshSizeInput.setFixedHeight(20);
    _meshSizeInput.setAlignment(Qt::AlignCenter);
    _meshSizeLayout3.addWidget(&_radio_3);
    _meshSizeLayout3.addWidget(&_meshSizeInput);
    _meshSizeLayout3.addWidget(&_meshSizeLabel);
	
    QSpacerItem *hS1=new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    _meshSizeLayout3.addItem(hS1);
	
    _layout.addLayout(&_meshSizeLayout3);

    QSpacerItem *vS1=new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS1);

    connect(&_radio_1, &QAbstractButton::toggled, this, &MeshProperties::radioButton1Toggled);
    connect(&_radio_2, &QAbstractButton::toggled, this, &MeshProperties::radioButton2Toggled);
    connect(&_radio_3, &QAbstractButton::toggled, this, &MeshProperties::radioButton3Toggled);

    connect(&_meshSizeInput,
            &QLineEdit::editingFinished,
            this,
            &MeshProperties::editingFinished,
            Qt::UniqueConnection);

    _catchUserInputs=false;
    _radio_1.setChecked(true);
    _catchUserInputs=true;

    _meshSizeLabel.setText("mm");

    translate();
}

MeshProperties::~MeshProperties()
{
}

void MeshProperties::radioButton1Toggled(bool checked)
{
    if (!checked)
        return;

    _meshSizeInput.setEnabled(false);
    if (!_catchUserInputs)
        return;

    if (_volumes.empty())
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    SetVolumesMeshSize *action=new SetVolumesMeshSize(model, _volumes, 0.025);
    ActionsManager::instance()->exec(action);
}

void MeshProperties::radioButton2Toggled(bool checked)
{
    if (!checked)
        return;

    _meshSizeInput.setEnabled(false);
    if (!_catchUserInputs)
        return;

    if (_volumes.empty())
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    SetVolumesMeshSize *action=new SetVolumesMeshSize(model, _volumes, 0.200);
    ActionsManager::instance()->exec(action);
}

void MeshProperties::radioButton3Toggled(bool checked)
{
    if (!checked)
        return;

    _catchUserInputs=false;
    _meshSizeInput.setText("25");
    _catchUserInputs=true;

    _meshSizeInput.setEnabled(true);
}

void MeshProperties::translate()
{
    _catchUserInputs=false;
    _radio_1.setText(_tr("par défaut (25 mm)"));
    _radio_2.setText(_tr("sol (200 mm)"));
    _radio_3.setText(_tr("personnalisé"));
    _catchUserInputs=true;
    update();
}

void MeshProperties::statesChanged()
{
    _volumes=StatesManager::instance()->selectedVolumes();
    if (_volumes.empty())
        return;

    _catchUserInputs=false;
    
    if (_volumes.size()==1)
    {
        Volume *volume=_volumes.at(0);
        double s=volume->cellsize();
        if (s==0.025)
            _radio_1.setChecked(true);
        else if (s==0.200)
            _radio_2.setChecked(true);
        else
        {
            QLocale locale;
            _radio_3.setChecked(true);
            _meshSizeInput.setText(locale.toString(s*1000.0, 'f', 1));
        }
    }

    else
    {
        // Get common size.
        std::vector<double> mesh_sizes;
        for (unsigned int i=0 ; i<_volumes.size() ; i++)
            mesh_sizes.push_back(_volumes.at(i)->cellsize());

        ToolBox::uniquify(mesh_sizes);

        // No common size.
        if (mesh_sizes.size()!=1)
        {
            _radio_1.setAutoExclusive(false);
            _radio_2.setAutoExclusive(false);
            _radio_3.setAutoExclusive(false);

            _radio_1.setChecked(false);
            _radio_2.setChecked(false);
            _radio_3.setChecked(false);

            _radio_1.setAutoExclusive(true);
            _radio_2.setAutoExclusive(true);
            _radio_3.setAutoExclusive(true);
        }

        // Common size.
        else
        {
            double s=mesh_sizes.at(0);
            if (s==0.025)
                _radio_1.setChecked(true);
            else if (s==0.200)
                _radio_2.setChecked(true);
            else
            {
                QLocale locale;
                _radio_3.setChecked(true);
                _meshSizeInput.setText(locale.toString(s*1000.0, 'f', 1));
            }
        }
    }
    _catchUserInputs=true;
}

void MeshProperties::editingFinished()
{
    if (!_catchUserInputs)
        return;

    if (!isVisible())
        return;

    QString value=_meshSizeInput.text();
    value.replace(",", ".");

    double v=value.toDouble()/1000.0;

    if (_volumes.empty())
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    SetVolumesMeshSize *action=new SetVolumesMeshSize(model, _volumes, v);
    ActionsManager::instance()->exec(action);
}
