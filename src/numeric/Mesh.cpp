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

#include <numeric/Mesh.h>
#include <numeric/Cell.h>

Mesh::Mesh()
{
}

Mesh::Mesh(const Mesh &mesh)
{
    clean();

    for (unsigned int i=0 ; i<mesh.size() ; i++)
    {
        Cell *cell  = mesh.at(i);
        Cell *nCell = new Cell(*cell);

        push_back(nCell);
    }
}

Mesh::~Mesh()
{
    clean();
}

void Mesh::clean()
{
    for (unsigned int i=0 ; i<size() ; i++)
        delete at(i);
    resize(0);
}
