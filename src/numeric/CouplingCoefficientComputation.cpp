// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2020, Cl�ment MARCEL.
//
// This file is part of conducte� software.
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

#include <numeric/CouplingCoefficientComputation.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>
#include <physics/Environment.h>
#include <model/Surface.h>
#include <algorithm>
#include <cmath>

using namespace std;

CouplingCoefficientComputation::CouplingCoefficientComputation(Model *model, Model::ModelComputationCallback cb, void *caller):
    _model(*model),
    _callback(cb),
    _caller(caller)
{
    _model.setCallback(0, 0);
}

CouplingCoefficientComputation::~CouplingCoefficientComputation()
{
}

std::vector<EnvironmentResult> CouplingCoefficientComputation::compute()
{
    std::vector<EnvironmentResult> results;
    std::vector<CouplingCoefficient> coefficients;

    // Get used environments.
    std::vector<std::string> environments=getEnvironments();
    unsigned int n=environments.size();
    unsigned int N=n*(n-1)/2;

    // Catch one or less environment.
    if (n<=1)
        return results;

    // 2 environments.
    if (n==2)
    {
        // Get environments handles.
        std::vector<std::string> envs=getNotNullEnvironments(1, environments);

        // Compute simulation.
        if (_callback)
            _callback(Model::CouplingCoefficients, 1, N, _caller);
        double flux=computeEnvironmentSimulation(envs);

        if (_model.isAborted())
            return results;

        // Save 1D flux.
        double _flux_d1=_model.flux1d();

        // Store results.
        CouplingCoefficient c;
        c.setFirstEnvironment(environments.at(0));
        c.setSecondEnvironment(environments.at(1));
        c.setCouplingCoefficient(flux);
        coefficients.push_back(c);

        c.setType(CouplingCoefficient::CouplingCoefficient1d);
        c.setFirstEnvironment(environments.at(0));
        c.setSecondEnvironment(environments.at(1));
        c.setCouplingCoefficient(_flux_d1);
        coefficients.push_back(c);
    }

    // 3 environments.
    else if (n==3)
    {
        // Environment 1.
        if (_callback)
            _callback(Model::CouplingCoefficients, 1, N, _caller);
        std::vector<std::string> envs=getNotNullEnvironments(1, environments);
        double A=computeEnvironmentSimulation(envs);
        double A_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 2.
        if (_callback)
            _callback(Model::CouplingCoefficients, 2, N, _caller);
        envs=getNotNullEnvironments(2, environments);
        double B=computeEnvironmentSimulation(envs);
        double B_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 3.
        if (_callback)
            _callback(Model::CouplingCoefficients, 3, N, _caller);
        envs=getNotNullEnvironments(3, environments);
        double C=computeEnvironmentSimulation(envs);
        double C_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Store results.
        CouplingCoefficient c13;
        c13.setFirstEnvironment(environments.at(0));
        c13.setSecondEnvironment(environments.at(2));
        c13.setCouplingCoefficient(0.5*(A-B+C));
        coefficients.push_back(c13);

        c13.setType(CouplingCoefficient::CouplingCoefficient1d);
        c13.setFirstEnvironment(environments.at(0));
        c13.setSecondEnvironment(environments.at(2));
        c13.setCouplingCoefficient(0.5*(A_prime-B_prime+C_prime));
        coefficients.push_back(c13);

        CouplingCoefficient c12;
        c12.setFirstEnvironment(environments.at(0));
        c12.setSecondEnvironment(environments.at(1));
        c12.setCouplingCoefficient(0.5*(A+B-C));
        coefficients.push_back(c12);

        c12.setType(CouplingCoefficient::CouplingCoefficient1d);
        c12.setFirstEnvironment(environments.at(0));
        c12.setSecondEnvironment(environments.at(1));
        c12.setCouplingCoefficient(0.5*(A_prime+B_prime-C_prime));
        coefficients.push_back(c12);

        CouplingCoefficient c23;
        c23.setFirstEnvironment(environments.at(1));
        c23.setSecondEnvironment(environments.at(2));
        c23.setCouplingCoefficient(0.5*(-A+B+C));
        coefficients.push_back(c23);

        c23.setType(CouplingCoefficient::CouplingCoefficient1d);
        c23.setFirstEnvironment(environments.at(1));
        c23.setSecondEnvironment(environments.at(2));
        c23.setCouplingCoefficient(0.5*(-A_prime+B_prime+C_prime));
        coefficients.push_back(c23);
    }

    // 4 environments.
    else if (n==4)
    {
        // Environment 1.
        if (_callback)
            _callback(Model::CouplingCoefficients, 1, N, _caller);
        std::vector<std::string> envs=getNotNullEnvironments(1, environments);
        double A=computeEnvironmentSimulation(envs);
        double A_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 2.
        if (_callback)
            _callback(Model::CouplingCoefficients, 2, N, _caller);
        envs=getNotNullEnvironments(2, environments);
        double B=computeEnvironmentSimulation(envs);
        double B_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 3.
        if (_callback)
            _callback(Model::CouplingCoefficients, 3, N, _caller);
        envs=getNotNullEnvironments(3, environments);
        double C=computeEnvironmentSimulation(envs);
        double C_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 4.
        if (_callback)
            _callback(Model::CouplingCoefficients, 4, N, _caller);
        envs=getNotNullEnvironments(4, environments);
        double D=computeEnvironmentSimulation(envs);
        double D_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 1+2.
        if (_callback)
            _callback(Model::CouplingCoefficients, 5, N, _caller);
        envs=getNotNullEnvironments(5, environments);
        double E=computeEnvironmentSimulation(envs);
        double E_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Environment 1+3.
        if (_callback)
            _callback(Model::CouplingCoefficients, 6, N, _caller);
        envs=getNotNullEnvironments(6, environments);
        double F=computeEnvironmentSimulation(envs);
        double F_prime=_model.flux1d();

        if (_model.isAborted())
            return results;

        // Store results.
        CouplingCoefficient c12;
        c12.setFirstEnvironment(environments.at(0));
        c12.setSecondEnvironment(environments.at(1));
        c12.setCouplingCoefficient(0.5*(A+B-E));
        coefficients.push_back(c12);

        c12.setType(CouplingCoefficient::CouplingCoefficient1d);
        c12.setFirstEnvironment(environments.at(0));
        c12.setSecondEnvironment(environments.at(1));
        c12.setCouplingCoefficient(0.5*(A_prime+B_prime-E_prime));
        coefficients.push_back(c12);

        CouplingCoefficient c13;
        c13.setFirstEnvironment(environments.at(0));
        c13.setSecondEnvironment(environments.at(2));
        c13.setCouplingCoefficient(0.5*(A+C-F));
        coefficients.push_back(c13);

        c13.setType(CouplingCoefficient::CouplingCoefficient1d);
        c13.setFirstEnvironment(environments.at(0));
        c13.setSecondEnvironment(environments.at(2));
        c13.setCouplingCoefficient(0.5*(A_prime+C_prime-F_prime));
        coefficients.push_back(c13);

        CouplingCoefficient c14;
        c14.setFirstEnvironment(environments.at(0));
        c14.setSecondEnvironment(environments.at(3));
        c14.setCouplingCoefficient(0.5*(E+F-B-C));
        coefficients.push_back(c14);

        c14.setType(CouplingCoefficient::CouplingCoefficient1d);
        c14.setFirstEnvironment(environments.at(0));
        c14.setSecondEnvironment(environments.at(3));
        c14.setCouplingCoefficient(0.5*(E_prime+F_prime-B_prime-C_prime));
        coefficients.push_back(c14);

        CouplingCoefficient c23;
        c23.setFirstEnvironment(environments.at(1));
        c23.setSecondEnvironment(environments.at(2));
        c23.setCouplingCoefficient(0.5*(E+F-A-D));
        coefficients.push_back(c23);

        c23.setType(CouplingCoefficient::CouplingCoefficient1d);
        c23.setFirstEnvironment(environments.at(1));
        c23.setSecondEnvironment(environments.at(2));
        c23.setCouplingCoefficient(0.5*(E_prime+F_prime-A_prime-D_prime));
        coefficients.push_back(c23);

        CouplingCoefficient c24;
        c24.setFirstEnvironment(environments.at(1));
        c24.setSecondEnvironment(environments.at(3));
        c24.setCouplingCoefficient(0.5*(B+D-F));
        coefficients.push_back(c24);

        c24.setType(CouplingCoefficient::CouplingCoefficient1d);
        c24.setFirstEnvironment(environments.at(1));
        c24.setSecondEnvironment(environments.at(3));
        c24.setCouplingCoefficient(0.5*(B_prime+D_prime-F_prime));
        coefficients.push_back(c24);

        CouplingCoefficient c34;
        c34.setFirstEnvironment(environments.at(2));
        c34.setSecondEnvironment(environments.at(3));
        c34.setCouplingCoefficient(0.5*(D+C-E));
        coefficients.push_back(c34);

        c34.setType(CouplingCoefficient::CouplingCoefficient1d);
        c34.setFirstEnvironment(environments.at(2));
        c34.setSecondEnvironment(environments.at(3));
        c34.setCouplingCoefficient(0.5*(D_prime+C_prime-E_prime));
        coefficients.push_back(c34);
    }

    // More environments: not yet implemented.
    else
        return results;

    // Extract results.
    for (unsigned int i=0 ; i<coefficients.size() ; i++)
    {
        CouplingCoefficient &c=coefficients.at(i);
        if (c.type()!=CouplingCoefficient::CouplingCoefficient2d)
            continue;

        EnvironmentResult env;
        env._environment1Id=c.firstEnvironmentId();
        env._environment2Id=c.secondEnvironmentId();
        env._couplingCoefficient=getCoefficient(coefficients, env._environment1Id, env._environment2Id, CouplingCoefficient::CouplingCoefficient2d)
                                -getCoefficient(coefficients, env._environment1Id, env._environment2Id, CouplingCoefficient::CouplingCoefficient1d);

        results.push_back(env);
    }

    return results;
}

std::vector<std::string> CouplingCoefficientComputation::getEnvironments() const
{
    std::vector<std::string> environments;

    for (unsigned int i=0 ; i<_model.std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=_model.std::vector<Surface*>::at(i);
        if (!surface)
            continue;

        Environment *environment=surface->environment();
        if (!environment)
            continue;

        environments.push_back(environment->name());
    }

    std::sort(environments.begin(), environments.end());
    std::vector<std::string>::iterator it=std::unique(environments.begin(), environments.end());
    environments.resize(std::distance(environments.begin(),it));

    return environments;
}

std::vector<std::string> CouplingCoefficientComputation::getNotNullEnvironments(int simulation_number, const std::vector<std::string> &environments) const
{
    std::vector<std::string> envs;

    if (simulation_number<=0 || environments.empty())
        return envs;

    if (simulation_number<=int(environments.size()))
    {
        envs.push_back(environments.at(simulation_number-1));
        return envs;
    }

    envs.push_back(environments.at(0));
    std::vector<std::string> children=environments;
    children.erase(children.begin());

    std::vector<std::string> envs2=getNotNullEnvironments(simulation_number-environments.size(), children);
    for (unsigned int i=0 ; i<envs2.size() ; i++)
        envs.push_back(envs2.at(i));

    return envs;
}

double CouplingCoefficientComputation::computeEnvironmentSimulation(const std::vector<std::string> &unit_environments)
{
    // Set all temperatures to 0�C or 1�C.
    for (unsigned int i=0 ; i<_model.std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=_model.std::vector<Surface*>::at(i);
        if (!surface)
            continue;

        BoundaryCondition *bc=surface->boundaryCondition();
        if (!bc)
            continue;

        Environment *environment=surface->environment();
        if (!environment)
            continue;

        double temperature=environmentTemperature(environment->name(), unit_environments);
        surface->setFixedTemperature(temperature);
    }

    Model *model1d=_model.model1d();
    if (model1d)
    {
        for (unsigned int i=0 ; i<model1d->std::vector<Surface*>::size() ; i++)
        {
            Surface *surface=model1d->std::vector<Surface*>::at(i);
            if (!surface)
                continue;

            BoundaryCondition *bc=surface->boundaryCondition();
            if (!bc)
                continue;

            Environment *environment=surface->environment();
            if (!environment)
                continue;

            double temperature=environmentTemperature(environment->name(), unit_environments);
            surface->setFixedTemperature(temperature);
        }
    }

    // Compute model.
    _model.compute(0, false);

    // Restore boundary conditions.
    for (unsigned int i=0 ; i<_model.std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=_model.std::vector<Surface*>::at(i);
        if (!surface)
            continue;

        surface->unsetFixedTemperature();
    }

    if (model1d)
    {
        for (unsigned int i=0 ; i<model1d->std::vector<Surface*>::size() ; i++)
        {
            Surface *surface=model1d->std::vector<Surface*>::at(i);
            if (!surface)
                continue;

            surface->unsetFixedTemperature();
        }
    }

    // Return 2D flux.
    return _model.flux2d();
}

double CouplingCoefficientComputation::environmentTemperature(const std::string &id, const std::vector<std::string> &unit_environments) const
{
    for (unsigned int j=0 ; j<unit_environments.size() ; j++)
    {
        std::string unit_env=unit_environments.at(j);
        if (unit_env==id)
            return 1.0;
    }
    return 0.0;
}

double CouplingCoefficientComputation::getCoefficient(const std::vector<CouplingCoefficient> &coefficients, const std::string &env1, const std::string &env2, CouplingCoefficient::Type type) const
{
    for (unsigned int i=0 ; i<coefficients.size() ; i++)
    {
        const CouplingCoefficient &c=coefficients.at(i);
        if (c.type()!=type)
            continue;

        if (c.firstEnvironmentId()==env1 && c.secondEnvironmentId()==env2)
            return c.couplingCoefficient();

        if (c.firstEnvironmentId()==env2 && c.secondEnvironmentId()==env1)
            return c.couplingCoefficient();
    }
    return 0.0;
}
