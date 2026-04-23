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

#include <actions/model/UpdateAnnotationPoints.h>
#include <model/Model.h>
#include <Log.h>

UpdateAnnotationPoints::UpdateAnnotationPoints(Model *model, const AnnotationSelections& selections, const std::vector<std::string>& ids, double begin_dx, double begin_dy, double end_dx, double end_dy, double position_dx, double position_dy, double angle_dx, double angle_dy):
    _ids(ids),
    _model(model),
    _begin_dx(begin_dx),
    _begin_dy(begin_dy),
    _end_dx(end_dx),
    _end_dy(end_dy),
    _position_dx(position_dx),
    _position_dy(position_dy),
    _selections(selections),
    _angle_dx(angle_dx),
    _angle_dy(angle_dy)
{
    _alterResults = false;
}

UpdateAnnotationPoints::~UpdateAnnotationPoints()
{
}

void UpdateAnnotationPoints::exec()
{
    if (_ids.size() != _selections.size())
    {
        LOG_WARNING("_ids.size() != _selections.size()");
        return;
    }

    for (unsigned int i=0 ; i<_ids.size() ; i++)
    {
        Annotation *annotation = _model->getAnnotationById(_ids.at(i));
        Annotation::SelectionPart selection = _selections.at(i);

        if (annotation)
        {
            bool b_line = (selection & Annotation::Whole) || (selection & Annotation::Lines);
            bool b_text = (selection & Annotation::Whole) || (selection & Annotation::Text);

            if (b_line)
            {
                Point p = annotation->begin();
                p.setX(p.x() + _begin_dx);
                p.setY(p.y() + _begin_dy);
                annotation->setBegin(p);

                p = annotation->end();
                p.setX(p.x() + _end_dx);
                p.setY(p.y() + _end_dy);
                annotation->setEnd(p);

                p = annotation->angleCenter();
                p.setX(p.x() + _angle_dx);
                p.setY(p.y() + _angle_dy);
                annotation->setAngleCenter(p);
            }

            if (b_text)
            {
                Point p = annotation->position();
                p.setX(p.x() + _position_dx);
                p.setY(p.y() + _position_dy);
                annotation->setPosition(p);
            }

            annotation->updateText();
        }
    }
}

void UpdateAnnotationPoints::undo()
{
    if (_ids.size() != _selections.size())
    {
        LOG_WARNING("_ids.size() != _selections.size()");
        return;
    }

    for (unsigned int i=0 ; i<_ids.size() ; i++)
    {
        Annotation *annotation = _model->getAnnotationById(_ids.at(i));
        Annotation::SelectionPart selection = _selections.at(i);

        if (annotation)
        {
            bool b_line = (selection & Annotation::Whole) || (selection & Annotation::Lines);
            bool b_text = (selection & Annotation::Whole) || (selection & Annotation::Text);

            if (b_line)
            {
                Point p = annotation->begin();
                p.setX(p.x() - _begin_dx);
                p.setY(p.y() - _begin_dy);
                annotation->setBegin(p);

                p = annotation->end();
                p.setX(p.x() - _end_dx);
                p.setY(p.y() - _end_dy);
                annotation->setEnd(p);

                p = annotation->angleCenter();
                p.setX(p.x() - _angle_dx);
                p.setY(p.y() - _angle_dy);
                annotation->setAngleCenter(p);
            }

            if (b_text)
            {
                Point p = annotation->position();
                p.setX(p.x() - _position_dx);
                p.setY(p.y() - _position_dy);
                annotation->setPosition(p);
            }

            annotation->updateText();
        }
    }
}
