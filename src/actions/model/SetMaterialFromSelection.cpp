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

#include <actions/model/SetMaterialFromSelection.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <geometry/Line.h>
#include <model/Volume.h>
#include <model/Model.h>

SetMaterialFromSelection::SetMaterialFromSelection(Model *model, const std::string &id, double x, double y, double width, double height):
    _model(model),
    _id(id),
    _x(x),
    _y(y),
    _width(width),
    _height(height)
{
}

SetMaterialFromSelection::~SetMaterialFromSelection()
{
}

void SetMaterialFromSelection::exec()
{
    storePhysics();

    std::vector<Volume*> volumes = availableVolumes();

    for (unsigned int i=0 ; i<volumes.size() ; i++)
    {
        Volume *volume = volumes.at(i);
        if (!volume)
            continue;

        Material *material = Materials::instance()->materialFromId(_id);

        volume->setMaterial(material);
    }
}

void SetMaterialFromSelection::undo()
{
    restorePhysics();
}

bool SetMaterialFromSelection::isValid() const
{
    return (availableVolumes().size() > 0);
}

void SetMaterialFromSelection::storePhysics()
{
    _storedData.clear();

    for (unsigned int i=0 ; i<_model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume = _model->std::vector<Volume*>::at(i);
        if (!volume)
            continue;

        std::string id = volume->id();

        Material *material = volume->material();
        std::string materialId;
        if (material)
            materialId = material->id();

        _storedData[id] = materialId;
    }
}

void SetMaterialFromSelection::restorePhysics()
{
    for (std::map<std::string, std::string>::iterator it=_storedData.begin() ; it!=_storedData.end() ; it++)
    {
        std::string id = it->first;
        std::string materialId = it->second;

        // Get volume & material.
        Volume *volume = _model->getVolumeById(id);
        if (!volume)
            continue;

        Material *material = Materials::instance()->materialFromId(materialId);

        volume->setMaterial(material);
    }
}

std::vector<Volume *> SetMaterialFromSelection::availableVolumes() const
{
    std::vector<Volume *> volumes;

    for (unsigned int v=0 ; v<_model->std::vector<Volume*>::size() ; v++)
    {
        Volume *volume = _model->std::vector<Volume*>::at(v);
        volume->computeLines();

        std::vector<Line*> lines = volume->getLines();

        bool addVolume = true;

        for (unsigned int j=0 ; j<lines.size() ; j++)
        {
            Line *line = lines.at(j);

            double x0 = line->begin().x();
            double y0 = line->begin().y();

            double x1 = line->end().x();
            double y1 = line->end().y();

            if (x0 < _x)
            {
                addVolume = false;
                break;
            }

            if (y0 < _y)
            {
                addVolume = false;
                break;
            }

            if (x0 > _x + _width)
            {
                addVolume = false;
                break;
            }

            if (y0 > _y + _height)
            {
                addVolume = false;
                break;
            }

            if (x1 < _x)
            {
                addVolume = false;
                break;
            }

            if (y1 < _y)
            {
                addVolume = false;
                break;
            }

            if (x1 > _x + _width)
            {
                addVolume = false;
                break;
            }

            if (y1 > _y + _height)
            {
                addVolume = false;
                break;
            }
        }

        if (addVolume)
            volumes.push_back(volume);
    }

    return volumes;
}
