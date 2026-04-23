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

#ifndef _THERM_IMPORTER_H
#define _THERM_IMPORTER_H

#include <vector>
#include <string>
#include <map>

class Surface;
class Volume;

class ThermImporter
{
    
public:

    ThermImporter();
    virtual ~ThermImporter();

    void loadThermFile(const std::string &content);
    std::string toThermFile(const std::vector<Volume*> &volumes, const std::vector<Surface*> &surfaces) const;

protected:

    virtual void addPolygon(const std::vector<double> x_values, const std::vector<double> y_values, const std::string &materialId)=0;
    virtual void finalizePolygons()=0;
    virtual void addSurface(double x0, double y0, double x1, double y1, const std::string &boundaryId)=0;

private:

    unsigned int headerLength(const std::string &content) const;
    unsigned char readValueAtPosition(const std::string &content, unsigned int offset) const;
    std::string readValue(const std::string &content, unsigned int offset, unsigned int length) const;
    float readFloat(const std::string &content, unsigned int offset) const;

    std::string writeFloat(float value) const;
    void appendString(std::string &content, const std::string &data) const;

    void readMaterial(const std::string &content, unsigned int offset);
    void readPolygon(const std::string &content, unsigned int offset);
    void readBoundaryCondition(const std::string &content, unsigned int offset);
    void readSurface(const std::string &content, unsigned int offset);

    std::map<std::string, std::string>  _materialsMap;
    std::map<std::string, std::string>  _boundariesMap;

};

#endif
