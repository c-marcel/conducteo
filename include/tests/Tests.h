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

#ifndef TESTS_H
#define TESTS_H

#include <vector>
#include <string>

class Surface;
class Model;
class Path;

class Tests
{

public:

    std::string exec();
    std::string rotation();
    std::string translation();
    std::string scaling();
    std::string closedVolume();
    std::string lineIntersection();
    std::string lineContainsPoint();
    std::string ellipsePointAtAngle();
    std::string volumeContainsPoint();
    std::string createSurfaces();
    std::string createSurfaces2();
    std::string createSurfaces3();
    std::string createSurfaces4();
    std::string circleLineIntersection();
    std::string c2dSurfacesCreation();
    std::string en10211Tests();
    std::string en10077Tests();
    std::string psiCoefficients();
    std::string splitCells();

    std::string pluginEn13370();

private:

    bool isEqual(double value, double reference, double precision=1.0e-12) const;
    bool containsLine(const std::vector<Surface*> &surfaces, double x0, double y0, double x1, double y1) const;
    bool containsPath(const std::vector<Surface*> &surfaces, Path *ref) const;
    bool surfacesAreEquivalent(const std::vector<Surface*> &surfaces1, const std::vector<Surface*> &surfaces2) const;
    bool compareModelSurfacesCreation(const std::string &xml) const;
    bool comparePsiCoefficient(const std::string &xml, double psi, double precision) const;
    bool comparePsiCoefficient(const std::string &xml, const std::string &env1, const std::string &env2, double psi, double precision) const;
    bool checkTemperature(Model *model, double x, double y, double reference, double precision) const;
    double readTemperature(Model *model, double x, double y) const;
    double readFlux(Model *model, double x, double y) const;
    double readPsiCoefficient(const std::string &xml) const;
    bool checkFlux(Model *model, double x, double y, double reference, double precision) const;

};

#endif
