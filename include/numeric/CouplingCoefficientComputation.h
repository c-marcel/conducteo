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

#ifndef _COUPLING_COEFFICIENT_COMPUTATION_H
#define _COUPLING_COEFFICIENT_COMPUTATION_H

#include <numeric/CouplingCoefficient.h>
#include <numeric/EnvironmentResult.h>
#include <model/Model.h>
#include <vector>

class CouplingCoefficientComputation
{

public:

    CouplingCoefficientComputation(Model *model, Model::ModelComputationCallback cb, void *caller);
    virtual ~CouplingCoefficientComputation();

    std::vector<EnvironmentResult> compute();

private:

    std::vector<std::string> getEnvironments() const;
    std::vector<std::string> getNotNullEnvironments(int simulation_number, const std::vector<std::string> &environments) const;
    double computeEnvironmentSimulation(const std::vector<std::string> &unit_environments);
    double environmentTemperature(const std::string &id, const std::vector<std::string> &unit_environments) const;
    double getCoefficient(const std::vector<CouplingCoefficient> &coefficients, const std::string &env1, const std::string &env2, CouplingCoefficient::Type type) const;

    Model                               _model;
    Model::ModelComputationCallback     _callback;
    void                               *_caller;

};

#endif
