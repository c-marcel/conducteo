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

#ifndef _UPDATE_ANNOTATION_POINTS_H
#define _UPDATE_ANNOTATION_POINTS_H

#include <actions/UserAction.h>
#include <model/Annotation.h>
#include <vector>
#include <string>

typedef std::vector<Annotation::SelectionPart> AnnotationSelections;
class Model;

class UpdateAnnotationPoints: public UserAction
{
public:
    UpdateAnnotationPoints(Model *model, const AnnotationSelections& selections, const std::vector<std::string>& ids, double begin_dx, double begin_dy, double end_dx, double end_dy, double position_dx, double position_dy, double angle_dx, double angle_dy);
    virtual ~UpdateAnnotationPoints();

    void exec();
    void undo();

private:
    AnnotationSelections        _selections;
    Model                      *_model;
    std::vector<std::string>    _ids;
    double                      _begin_dx;
    double                      _begin_dy;
    double                      _end_dx;
    double                      _end_dy;
    double                      _position_dx;
    double                      _position_dy;
    double                      _angle_dx;
    double                      _angle_dy;
};

#endif
