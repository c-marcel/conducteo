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

#include <actions/model/DeleteDxf.h>
#include <actions/ActionsManager.h>
#include <model/Model.h>

DeleteDxf::DeleteDxf(Model *model, DxfContent *dxf):
    _model(model),
    _dxfId(dxf->id()),
    _oldDxf(*dxf)
{
    _alterResults = false;
}

DeleteDxf::~DeleteDxf()
{
}

void DeleteDxf::exec()
{
    DxfContent *dxf=_model->getDxfById(_dxfId);
    _model->removeDxf(dxf);
}
    
void DeleteDxf::undo()
{
    DxfContent *dxf=new DxfContent(_oldDxf);
    dxf->setId(_dxfId);
    _model->std::vector<DxfContent*>::push_back(dxf);
}
