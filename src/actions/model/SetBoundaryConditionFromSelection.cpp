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

#include <actions/model/SetBoundaryConditionFromSelection.h>
#include <databases/BoundaryConditions.h>
#include <model/Surface.h>
#include <model/Model.h>

SetBoundaryConditionFromSelection::SetBoundaryConditionFromSelection(Model *model, const std::string &id, double x, double y, double width, double height):
    _model(model),
    _id(id),
    _x(x),
    _y(y),
    _width(width),
    _height(height)
{
}

SetBoundaryConditionFromSelection::~SetBoundaryConditionFromSelection()
{
}

void SetBoundaryConditionFromSelection::exec()
{
    storePhysics(_model);

    std::vector<Surface *> surfaces = availableSurfaces();

    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Surface *surface = surfaces.at(i);
        surface->setBoundaryCondition(BoundaryConditions::instance()->bcFromId(_id));
    }
}

void SetBoundaryConditionFromSelection::undo()
{
    restorePhysics(_model);
}

bool SetBoundaryConditionFromSelection::isValid() const
{
    return (availableSurfaces().size() > 0);
}

std::vector<Surface *> SetBoundaryConditionFromSelection::availableSurfaces() const
{
    std::vector<Surface *> surfaces;

    for (unsigned int i=0 ; i<_model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface = _model->std::vector<Surface*>::at(i);

        bool addSurface = true;

        for (unsigned int j=0 ; j<surface->size() ; j++)
        {
            Line *line = surface->at(j);

            double x0 = line->begin().x();
            double y0 = line->begin().y();

            double x1 = line->end().x();
            double y1 = line->end().y();

            if (x0 < _x)
            {
                addSurface = false;
                break;
            }

            if (y0 < _y)
            {
                addSurface = false;
                break;
            }

            if (x0 > _x + _width)
            {
                addSurface = false;
                break;
            }

            if (y0 > _y + _height)
            {
                addSurface = false;
                break;
            }

            if (x1 < _x)
            {
                addSurface = false;
                break;
            }

            if (y1 < _y)
            {
                addSurface = false;
                break;
            }

            if (x1 > _x + _width)
            {
                addSurface = false;
                break;
            }

            if (y1 > _y + _height)
            {
                addSurface = false;
                break;
            }
        }

        if (addSurface)
            surfaces.push_back(surface);
    }

    return surfaces;
}
