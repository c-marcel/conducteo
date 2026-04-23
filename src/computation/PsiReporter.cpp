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

#include <computation/PsiReporter.h>
#include <databases/Environments.h>
#include <physics/Environment.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <tools/ToolBox.h>
#include <model/Model.h>
#include <QPalette>
#include <QPainter>
#include <QLocale>
#include <Log.h>

PsiReporter::PsiReporter(QWidget *parent):
    QWidget(parent),
    _layout(this)
{
    QSpacerItem *vS1=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout.addItem(vS1);

    QFont f = font();
    f.setBold(true);
    f.setUnderline(true);
    f.setPixelSize(12);

    QFont defFont = font();
    defFont.setPixelSize(11);

#ifdef _WIN32
    int textHeight = 20;
#else
    int textHeight = 14;
#endif

    _fluxTitle.setFont(f);
    _fluxTitle.setMinimumWidth(120);
    _layout.addWidget(&_fluxTitle);

    _1DfluxTitle.setMinimumWidth(120);
    _1DfluxTitle.setFont(defFont);
    _1DFluxLayout.setContentsMargins(10, 0, 0, 0);
    _1DfluxTitle.setMinimumHeight(textHeight);
    _1DfluxValue.setMinimumHeight(textHeight);
    _1DfluxValue.setFont(defFont);
    _1DFluxLayout.addWidget(&_1DfluxTitle);
    _1DFluxLayout.addWidget(&_1DfluxValue);
    _1DFluxLayout.addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _layout.addLayout(&_1DFluxLayout);

    _2DfluxTitle.setMinimumWidth(120);
    _2DfluxTitle.setFont(defFont);
    _2DFluxLayout.setContentsMargins(10, 0, 0, 0);
    _2DfluxTitle.setMinimumHeight(textHeight);
    _2DfluxValue.setMinimumHeight(textHeight);
    _2DfluxValue.setFont(defFont);
    _2DFluxLayout.addWidget(&_2DfluxTitle);
    _2DFluxLayout.addWidget(&_2DfluxValue);
    _2DFluxLayout.addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _layout.addLayout(&_2DFluxLayout);

    // Condensation.
    _condensationTitle.setFont(f);
    _condensationTitle.setMinimumWidth(120);

    _condensationValue.setMinimumWidth(120);
    _condensationValue.setAlignment(Qt::AlignCenter);

    _layout.addWidget(&_condensationTitle);
    _layout.addWidget(&_condensationValue);

    f.setUnderline(false);
    _psiTitle.setFont(f);
    _psiValue.setFont(f);

    _psiTitle.setMinimumWidth(130);
    _psiTitle.setMinimumHeight(textHeight);
    _psiValue.setMinimumHeight(textHeight);
    _psiLayout.addWidget(&_psiTitle);
    _psiLayout.addWidget(&_psiValue);
    _psiLayout.addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    _layout.addLayout(&_psiLayout);

    _couplingLayout.addWidget(&_coupling1);
    _couplingLayout.addWidget(&_coupling2);
    _couplingLayout.addWidget(&_coupling3);
    _couplingLayout.addWidget(&_coupling4);
    _couplingLayout.addWidget(&_coupling5);
    _couplingLayout.addWidget(&_coupling6);

    _couplingLayout.setAlignment(&_coupling1, Qt::AlignLeft);
    _couplingLayout.setAlignment(&_coupling2, Qt::AlignLeft);
    _couplingLayout.setAlignment(&_coupling3, Qt::AlignLeft);
    _couplingLayout.setAlignment(&_coupling4, Qt::AlignLeft);
    _couplingLayout.setAlignment(&_coupling5, Qt::AlignLeft);
    _couplingLayout.setAlignment(&_coupling6, Qt::AlignLeft);

    _globalCouplingLayout.addWidget(&_globalCoupling1);
    _globalCouplingLayout.addWidget(&_globalCoupling2);
    _globalCouplingLayout.addWidget(&_globalCoupling3);
    _globalCouplingLayout.addWidget(&_globalCoupling4);

    _repartitionLayout.addLayout(&_couplingLayout);
    _repartitionLayout.addLayout(&_globalCouplingLayout);

    _layout.addLayout(&_repartitionLayout);

    QSpacerItem *vS2=new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Minimum);
    _layout.addItem(vS2);

    _layout.setContentsMargins(50, 10, 10, 25);

    setMaximumWidth(500);
    setFixedHeight(120);

    translate();
    applyTheme();
}

PsiReporter::~PsiReporter()
{
}

void PsiReporter::setOutgoing1DFlux(double flux)
{
    QLocale locale;
    _1DfluxValue.setText(locale.toString(flux, 'f', 3)+" W/m");
}

void PsiReporter::setOutgoing2DFlux(double flux, const QString &unit)
{
    QLocale locale;
    _2DfluxValue.setText(locale.toString(flux, 'f', 3) + unit);
}

void PsiReporter::setPsiCoefficient(double psi)
{
    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    // Default: psi computation, equivalent lambda.
    QLocale locale;
    _psiValue.setText(locale.toString(psi, 'f', 3)+" W/(m.°C)");

    if (model->computationType() == Model::ThermalTransmission)
        _psiValue.setText(locale.toString(psi, 'f', 3)+" W/(m².°C)");
}

void PsiReporter::setCondensation(bool condensation)
{
    if (condensation)
        _condensation = "Oui";
    else
        _condensation = "Non";

    translate();
}

void PsiReporter::unset1DFluxAndPsiCoefficient()
{
    _1DfluxValue.setText("-");
    _psiValue.setText("-");
}

void PsiReporter::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);

    QRect rec(40, 0, width()-41, height()-15);

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

void PsiReporter::translate()
{
    _fluxTitle.setText(_tr("Flux sortant"));
    _1DfluxTitle.setText(_tr("Sans pont thermique :"));
    _2DfluxTitle.setText(_tr("Avec pont thermique :"));
    _psiTitle.setText(_tr("Coefficient ψ :"));

    _condensationTitle.setText(_tr("Condensation"));
    _condensationValue.setText(_tr(_condensation));

    Model *model = StatesManager::instance()->currentModel();

    if (model)
    {
        if (model->computationType()==Model::Flux || model->computationType() == Model::GlaserCondensation)
            _2DfluxTitle.setText(_tr("Modélisation :"));
        if (model->computationType() == Model::EquivalentThermalConductivity)
            _2DfluxTitle.setText(_tr("Conductivité équivalente :"));
        if (model->computationType()==Model::ThermalTransmission)
            _psiTitle.setText(_tr("Coefficient U :"));
    }

    update();
}

void PsiReporter::applyTheme()
{
    QPalette p;
    p.setColor(_psiTitle.foregroundRole(), QColor(236, 145, 16));

    _psiTitle.setPalette(p);
    _psiValue.setPalette(p);

    update();
}

void PsiReporter::statesChanged()
{
    LOG_INFO("States changed caught.");

    setVisible(StatesManager::instance()->simulationState()==StatesManager::Done);
    if (StatesManager::instance()->viewType()==StatesManager::DisplayReport)
        setVisible(false);

    Model *model=StatesManager::instance()->currentModel();
    if (model && StatesManager::instance()->simulationState()==StatesManager::Done)
    {
        setOutgoing2DFlux(model->flux2d(), " W/m");

        setOutgoing1DFlux(model->flux1d());
        if (model->computationType()==Model::ThermalBridge)
            setPsiCoefficient(model->psiCoefficient());
        else if (model->computationType()==Model::ThermalTransmission)
            setPsiCoefficient(model->transmissionCoefficient());
        else if (model->computationType()==Model::EquivalentThermalConductivity)
            setOutgoing2DFlux(model->equivalentThermalConductivity(), " W/(m.°C)");

        // Condensation.
        setCondensation(model->getCondensation());

        _condensationTitle.setVisible(model->computationType() == Model::GlaserCondensation);
        _condensationValue.setVisible(model->computationType() == Model::GlaserCondensation);

        // Environments.
        _environmentsResults=model->environmentResults();

        _coupling1.hide();
        _coupling2.hide();
        _coupling3.hide();
        _coupling4.hide();
        _coupling5.hide();
        _coupling6.hide();

        _1DfluxTitle.setVisible((model->computationType()!=Model::GlaserCondensation) && (model->computationType()!=Model::Flux) && (model->computationType() != Model::EquivalentThermalConductivity));
        _1DfluxValue.setVisible((model->computationType()!=Model::GlaserCondensation) && (model->computationType()!=Model::Flux) && (model->computationType() != Model::EquivalentThermalConductivity));

        _psiTitle.setVisible((model->computationType()!=Model::GlaserCondensation) && (model->computationType()!=Model::Flux) && (model->computationType() != Model::EquivalentThermalConductivity));
        _psiValue.setVisible((model->computationType()!=Model::GlaserCondensation) && (model->computationType()!=Model::Flux) && (model->computationType() != Model::EquivalentThermalConductivity));

        int h=120;
        if (model->computationType()==Model::Flux || model->computationType()==Model::EquivalentThermalConductivity)
            h=90;

        setFixedHeight(h + _environmentsResults.size()*25);

        CouplingCoefficientReporter *r=0;

        for (unsigned int i=0 ; i<_environmentsResults.size() ; i++)
        {
            EnvironmentResult &env=_environmentsResults.at(i);

            if (i==0)
                r=&_coupling1;
            else if (i==1)
                r=&_coupling2;
            else if (i==2)
                r=&_coupling3;
            else if (i==3)
                r=&_coupling4;
            else if (i==4)
                r=&_coupling5;
            else if (i==5)
                r=&_coupling6;

            if (!r)
                continue;

            r->setFirstEnvironment(env._environment1Id.c_str());
            r->setSecondEnvironment(env._environment2Id.c_str());
            r->setCoefficient(env._couplingCoefficient);

            Environment *env1=Environments::instance()->environmentFromName(env._environment1Id);
            Environment *env2=Environments::instance()->environmentFromName(env._environment2Id);

            if (!env1)
                continue;
            if (!env2)
                continue;

            QColor color;
            color.setRed(env1->red());
            color.setGreen(env1->green());
            color.setBlue(env1->blue());

            r->setFirstEnvColor(color);

            color.setRed(env2->red());
            color.setGreen(env2->green());
            color.setBlue(env2->blue());
            r->setSecondEnvColor(color);
            
            r->show();
        }

        // Global coupling.
        _globalCoupling1.hide();
        _globalCoupling2.hide();
        _globalCoupling3.hide();
        _globalCoupling4.hide();

        if (_environmentsResults.size() > 1)
        {
            // Env. 1
            setGlobalCoupling("1", &_globalCoupling1);

            // Env. 2
            setGlobalCoupling("2", &_globalCoupling2);

            // Env. 3
            setGlobalCoupling("3", &_globalCoupling3);

            // Env. 4
            setGlobalCoupling("4", &_globalCoupling4);
        }
    }

    // Interop.: set results as valid.
    if (model)
        model->setResultsValidity(StatesManager::instance()->simulationState()==StatesManager::Done);

    translate();
}

void PsiReporter::setGlobalCoupling(const std::string& id, CouplingCoefficientReporter *reporter)
{
    Environment *env=Environments::instance()->environmentFromName(id);
    if (env && ToolBox::useEnvironment(_environmentsResults, id))
    {
        QColor color;
        color.setRed(env->red());
        color.setGreen(env->green());
        color.setBlue(env->blue());

        reporter->setFirstEnvColor(color);
        reporter->setSecondEnvColor(color);
        reporter->setCoefficient(ToolBox::globalCouplingCoefficient(_environmentsResults, id));
        reporter->setFirstEnvironment(id.c_str());

        reporter->show();
    }
}

QString PsiReporter::condensation() const
{
    return _condensation;
}

void PsiReporter::setCondensation(const QString &condensation)
{
    _condensation = condensation;
}
