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

#include <actions/model/RemoveAnnotation.h>
#include <model/Model.h>

RemoveAnnotation::RemoveAnnotation(Model *model, const std::vector<Annotation*> &annotations):
    _model(model)
{
    _alterResults = false;

    _annotations.clear();
    for (unsigned int i=0 ; i<annotations.size() ; i++)
    {
        Annotation *a=annotations.at(i);
        Annotation b=*a;
        _annotations.push_back(b);
    }
}

RemoveAnnotation::~RemoveAnnotation()
{
}

void RemoveAnnotation::exec()
{
    for (unsigned int i=0 ; i<_annotations.size() ; i++)
        _model->removeAnnotation(_annotations.at(i).id());
}

void RemoveAnnotation::undo()
{
    for (unsigned int i=0 ; i<_annotations.size() ; i++)
    {
        Annotation *a=new Annotation(_annotations.at(i));
        _model->std::vector<Annotation*>::push_back(a);
    }
}
