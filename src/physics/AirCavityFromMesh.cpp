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

#include <physics/AirCavityFromMesh.h>
#include <databases/Materials.h>
#include <numeric/Cell.h>

AirCavityFromMesh::AirCavityFromMesh()
{
}

AirCavityFromMesh::~AirCavityFromMesh()
{
}

void AirCavityFromMesh::addCell(Cell *cell)
{
    cell->closeCell();
    push_back(cell);
}

void AirCavityFromMesh::finalize()
{
    if (empty())
        return;

    // Compute min & max bounds.
    double xmin=0.0;
    double ymin=0.0;
    double xmax=0.0;
    double ymax=0.0;

    for (unsigned int i=0 ; i<size() ; i++)
    {
        Cell *cell=at(i);
        if (cell->xMin()<xmin || i==0)
            xmin=cell->xMin();
        if (cell->yMin()<ymin || i==0)
            ymin=cell->yMin();
        if (cell->xMax()>xmax || i==0)
            xmax=cell->xMax();
        if (cell->yMax()>ymax || i==0)
            ymax=cell->yMax();
    }

    double width=xmax-xmin;
    double height=ymax-ymin;

    double area=0.0;
    for (unsigned int i=0 ; i<size() ; i++)
    {
        at(i)->closeCell();
        area+=at(i)->area();
    }

    setDimensions(width, height, area);

    // Apply conductivities to cells.
    for (unsigned int i=0 ; i<size() ; i++)
    {
        Cell *cell=at(i);
        cell->setHorizontalConductivity(horizontalConductivity());
        cell->setVerticalConductivity(verticalConductivity());
        cell->setMaterial(Materials::instance()->materialFromId("106D6720-211C-4090-8C60-7CED671C8580"));
    }
}
