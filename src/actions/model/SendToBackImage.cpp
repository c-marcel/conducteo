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

#include <actions/model/SendToBackImage.h>
#include <model/Image.h>
#include <model/Model.h>

SendToBackImage::SendToBackImage(Model *model, Image *image):
    _model(model),
    _imageId(image->id())
{
    _alterResults = false;

    _stackPosition=model->imageStackPosition(image);
}

SendToBackImage::~SendToBackImage()
{
}

void SendToBackImage::exec()
{
    Image *image=_model->getImageById(_imageId);
    if (image)
        _model->setImageStackPosition(image, 0);
}
void SendToBackImage::undo()
{
    Image *image=_model->getImageById(_imageId);
    if (image)
        _model->setImageStackPosition(image, _stackPosition);
}
