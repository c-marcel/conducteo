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

#include <actions/model/AddPolyline.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <geometry/Line.h>
#include <model/Volume.h>
#include <model/Model.h>

AddPolyline::AddPolyline(Model *model, const std::vector<Point> &points, const std::string &materialId):
    _model(model),
    _materialId(materialId),
    _points(points)
{
}

AddPolyline::~AddPolyline()
{
}

void AddPolyline::exec()
{
    storePhysics(_model);
    if (_points.empty())
        return;

    Volume *volume=new Volume;

    for (unsigned int i=0 ; i<_points.size() ; i++)
    {
        Point p1=_points.at(i);
        Point p2;
        if (i<_points.size()-1)
            p2=_points.at(i+1);
        else
            p2=_points.at(0);

        Line *line=new Line;
        line->setBegin(p1, 0);
        line->setEnd(p2, 0);
        volume->push_back(line);
    }

    volume->computeBounds();
    volume->computeLines();

    Material *material=Materials::instance()->materialFromId(_materialId);
    volume->setMaterial(material);

    _model->addVolume(volume);

    if (!_volumeId.empty())
        volume->setId(_volumeId);
    _volumeId=volume->id();
}

void AddPolyline::undo()
{
    _model->removeVolume(_volumeId);
    _model->recreateSurfaces();
    restorePhysics(_model);
}
