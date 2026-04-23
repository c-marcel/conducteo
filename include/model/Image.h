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

#ifndef _IMAGE_H
#define _IMAGE_H

#include <string>

class ErrorHandler;
class ImageData;

class Image
{

public:

    Image();
    Image(const Image &);
    virtual ~Image();
    
    void setX(double x);
    void setY(double y);
    void setWidth(double width);
    void setHeight(double height);
    void setData(const std::string &data);
    void setImageData(ImageData *data);
    void setSelected(bool selected);
    void setId(const std::string &id);
    
    double x() const;
    double y() const;
    double width() const;
    double height() const;
    std::string data() const;
    ImageData *imageData() const;
    bool selected() const;
    std::string id() const;

    std::string serialize() const;
    static Image *createFromXml(const std::string &xml, ErrorHandler *error);

private:

    double      _x;
    double      _y;
    double      _width;
    double      _height;
    std::string _id;
    std::string _data;
    ImageData  *_imageData;
    bool        _selected;
    
};

#endif
