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

#include <actions/model/AddImage.h>
#include <actions/ActionsManager.h>
#include <model/Image.h>
#include <model/Model.h>

AddImage::AddImage(Model *model, const std::string &data, double x, double y, double width, double height):
    _model(model),
    _x(x),
    _y(y),
    _width(width),
    _height(height),
    _data(data)
{
    _alterResults = false;
}

AddImage::~AddImage()
{
}

void AddImage::exec()
{
    Image *image=new Image;
    _model->std::vector<Image*>::push_back(image);

    image->setX(_x);
    image->setY(_y);
    image->setWidth(_width);
    image->setHeight(_height);
    image->setData(_data);

    if (!_imageId.empty())
        image->setId(_imageId);
    _imageId=image->id();
}

void AddImage::undo()
{
    _model->removeImage(_imageId);
}

Image *AddImage::image() const
{
    return _model->getImageById(_imageId);
}
