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

#ifndef _PSI_REPORTER_H
#define _PSI_REPORTER_H

#include <computation/CouplingCoefficientReporter.h>
#include <numeric/EnvironmentResult.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

class PsiReporter: public QWidget,
                   public Translatable,
                   public StatesObserver
{
public:
    PsiReporter(QWidget *parent=0);
    virtual ~PsiReporter();

    void setOutgoing1DFlux(double flux);
    void setOutgoing2DFlux(double flux, const QString &unit);
    void setPsiCoefficient(double psi);
    void setCondensation(bool condensation);
    void unset1DFluxAndPsiCoefficient();

    void translate();
    void applyTheme();
    void statesChanged();

    QString condensation() const;
    void setCondensation(const QString &condensation);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    void setGlobalCoupling(const std::string& id, CouplingCoefficientReporter *reporter);

    QVBoxLayout     _layout;
    QHBoxLayout     _1DFluxLayout;
    QHBoxLayout     _2DFluxLayout;
    QHBoxLayout     _psiLayout;
    QHBoxLayout     _repartitionLayout;
    QVBoxLayout     _couplingLayout;
    QVBoxLayout     _globalCouplingLayout;
    QLabel          _fluxTitle;
    QLabel          _1DfluxTitle;
    QLabel          _1DfluxValue;
    QLabel          _2DfluxTitle;
    QLabel          _2DfluxValue;
    QLabel          _psiTitle;
    QLabel          _psiValue;
    QLabel          _condensationTitle;
    QLabel          _condensationValue;

    std::vector<EnvironmentResult>      _environmentsResults;
    CouplingCoefficientReporter         _coupling1;
    CouplingCoefficientReporter         _coupling2;
    CouplingCoefficientReporter         _coupling3;
    CouplingCoefficientReporter         _coupling4;
    CouplingCoefficientReporter         _coupling5;
    CouplingCoefficientReporter         _coupling6;

    CouplingCoefficientReporter         _globalCoupling1;
    CouplingCoefficientReporter         _globalCoupling2;
    CouplingCoefficientReporter         _globalCoupling3;
    CouplingCoefficientReporter         _globalCoupling4;

    QString _condensation;
};

#endif
