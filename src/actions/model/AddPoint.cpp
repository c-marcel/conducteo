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

#include <actions/model/AddPoint.h>
#include <model/Volume.h>
#include <model/Model.h>

AddPoint::AddPoint(Model *model, const Point &point):
    _model(model),
    _point(point)
{
}

AddPoint::~AddPoint()
{
}

void AddPoint::exec()
{
    storePhysics(_model);
    _volumes.clear();

    for (unsigned int i=0 ; i<_model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=_model->std::vector<Volume*>::at(i);
        if (volume->pointOnBoundaries(_point) && !volume->pointOnCorner(_point))
        {
            std::string id=volume->id();
            _volumes.push_back(id);
            volume->addPoint(_point);

            volume->computeBounds();
            volume->computeLines();
        }
    }

    _model->recreateSurfaces();
}

void AddPoint::undo()
{
    restorePhysics(_model);

    for (unsigned int i=0 ; i<_volumes.size() ; i++)
    {
        std::string id=_volumes.at(i);
        Volume *volume=_model->getVolumeById(id);

        if (!volume)
            continue;

        volume->removePoint(_point);

        volume->computeBounds();
        volume->computeLines();
    }

    _model->recreateSurfaces();
}
