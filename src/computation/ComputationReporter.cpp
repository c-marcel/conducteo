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

#include <computation/ComputationReporter.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <QPainter>
#include <QLocale>
#include <Log.h>

ComputationReporter::ComputationReporter(QWidget *parent):
    QWidget(parent),
    _layout(this)
{
    QSpacerItem *vS1=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS1);

    QFont defFont = font();
    defFont.setPixelSize(11);

    _refinementTypeTitle.setFont(defFont);
    _refinementTypeTitle.setAlignment(Qt::AlignCenter);
    _layout.addWidget(&_refinementTypeTitle);

    _levelTitle.setFont(defFont);
    _levelValue.setFont(defFont);
    _levelTitle.setMinimumWidth(120);
    _levelValue.setMinimumWidth(300);
    _levelLayout.addWidget(&_levelTitle);
    _levelLayout.addWidget(&_levelValue);
    QSpacerItem *hS1=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _levelLayout.addItem(hS1);
    _layout.addLayout(&_levelLayout);

    _progress.setTextVisible(false);
    _layout.addWidget(&_progress);

    QSpacerItem *vS2=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS2);

    _cancel.setFont(defFont);
    _cancel.setAlignment(Qt::AlignCenter);
    _layout.addWidget(&_cancel);

    setFixedWidth(300);
    setFixedHeight(100);

    QFont f = font();
    f.setPixelSize(11);

    _refinementTypeTitle.setFont(f);
    _levelTitle.setFont(f);
    _levelValue.setFont(f);
    _cancel.setFont(f);

    connect(&_cancel,
            &QLabel::linkActivated,
            this,
            &ComputationReporter::onLinkActivated,
            Qt::UniqueConnection);
    translate();
    applyTheme();
}

ComputationReporter::~ComputationReporter()
{
}

void ComputationReporter::setLevel(int level, int total)
{
    LOG_INFO("Set level to: " << level << "/" << total);

    if (_type!=Model::CouplingCoefficients)
        _levelValue.setText(QString::number(level));
    else
        _levelValue.setText(QString::number(level)+"/"+QString::number(total));

    // Set progress bar as undefined value.
    _progress.setMinimum(0);
    _progress.setMaximum(0);
    _progress.setValue(0);

    _levelTitle.setVisible(level>0);
    _levelValue.setVisible(level>0);
}

void ComputationReporter::setRefinementType(Model::CallbackType type)
{
    LOG_INFO("Set refinement type to: " << type);

    _type=type;
    if (type==Model::PhysicalFlux)
        _refinementTypeTitle.setText(_tr("Calcul des flux thermiques"));
    else if (type==Model::CouplingCoefficients)
        _refinementTypeTitle.setText(_tr("Calcul des coefficients de couplage"));
    else if (type == Model::WithoutTBFlux)
        _refinementTypeTitle.setText(_tr("Calcul de la modélisation sans pont thermique"));
    else if (type == Model::GlaserComputation)
        _refinementTypeTitle.setText(_tr("Calcul des flux d'humidité"));

    if (type==Model::CouplingCoefficients)
        _levelTitle.setText(_tr("Simulation :"));
    else
        _levelTitle.setText(_tr("Niveau de raffinement :"));

    _levelTitle.setVisible(type != Model::GlaserComputation);
    _levelValue.setVisible(type != Model::GlaserComputation);
}

void ComputationReporter::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);

    QRect rec(0, 0, width()-1, height()-1);

    QColor backgroundColor(199, 229, 245);

    QColor borderColor(50, 75, 155);

    QBrush brush(backgroundColor);
    qp.setBrush(brush);

    QPen pen;
    pen.setColor(borderColor);
    pen.setWidth(3);
    qp.setPen(pen);
    qp.drawRect(rec);
}

void ComputationReporter::onLinkActivated(const QString &link)
{
    LOG_INFO("Abort link clicked.");
    if (link=="conducteo:computation/cancel")
        emit computationAbortRequested();
}

void ComputationReporter::translate()
{
    _levelTitle.setText(_tr("Niveau de raffinement :"));

    _cancel.setText("<a href=\"conducteo:computation/cancel\" style=\"color: #324b9b;\">" + _tr("Annuler") + "</a>");

    update();
}

void ComputationReporter::statesChanged()
{
    setVisible(StatesManager::instance()->simulationState()==StatesManager::InProgress);
}

void ComputationReporter::applyTheme()
{
    _cancel.setText("<a href=\"conducteo:computation/cancel\" style=\"color: #324b9b;\">" + _tr("Annuler") + "</a>");

    update();
}
