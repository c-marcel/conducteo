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

#include <actions/model/DeleteImage.h>
#include <actions/ActionsManager.h>
#include <model/Model.h>

DeleteImage::DeleteImage(Model *model, Image *image):
    _model(model),
    _imageId(image->id()),
    _oldImage(*image)
{
    _alterResults = false;
}

DeleteImage::~DeleteImage()
{
}

void DeleteImage::exec()
{
    Image *image=_model->getImageById(_imageId);
    _stackPosition=_model->imageStackPosition(image);
    _model->removeImage(image);
}
    
void DeleteImage::undo()
{
    Image *image=new Image(_oldImage);
    image->setId(_imageId);
    _model->insertImage(image, _stackPosition);
}
