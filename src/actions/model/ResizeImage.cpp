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

#include <actions/model/ResizeImage.h>
#include <actions/ActionsManager.h>
#include <model/Image.h>
#include <model/Model.h>

ResizeImage::ResizeImage(Model *model, Image *image, double dx, double dy, double dwidth, double dheight):
    _model(model),
    _imageId(image->id()),
    _dx(dx),
    _dy(dy),
    _dwidth(dwidth),
    _dheight(dheight)
{
    _alterResults = false;
}

ResizeImage::~ResizeImage()
{
}

void ResizeImage::exec()
{
    Image *image=_model->getImageById(_imageId);
    if (!image)
        return;

    image->setX(image->x()+_dx);
    image->setY(image->y()+_dy);

    image->setWidth(image->width()+_dwidth);
    image->setHeight(image->height()+_dheight);
}

void ResizeImage::undo()
{
    Image *image=_model->getImageById(_imageId);
    if (!image)
        return;

    image->setX(image->x()-_dx);
    image->setY(image->y()-_dy);

    image->setWidth(image->width()-_dwidth);
    image->setHeight(image->height()-_dheight);
}
