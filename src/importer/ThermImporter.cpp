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

#include <importer/ThermImporter.h>
#include <databases/BoundaryConditions.h>
#include <physics/BoundaryCondition.h>
#include <geometry/GeometryTools.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <model/Surface.h>
#include <tools/ToolBox.h>
#include <EncodedString.h>
#include <geometry/Path.h>
#include <model/Volume.h>
#include <cmath>
#include <string.h>
#include <stdlib.h>

#define header_begin_offset 95

ThermImporter::ThermImporter()
{
}

ThermImporter::~ThermImporter()
{
}

unsigned char ThermImporter::readValueAtPosition(const std::string &content, unsigned int offset) const
{
    if (offset>=content.size())
        return 0;
    return content.at(offset);
}

std::string ThermImporter::readValue(const std::string &content, unsigned int offset, unsigned int length) const
{
    std::string out;
    for (unsigned int i=0 ; i<length ; i++)
        out.push_back(readValueAtPosition(content, offset+i));
    return out;
}

float ThermImporter::readFloat(const std::string &content, unsigned int offset) const
{
    unsigned char *c=(unsigned char*)content.c_str();
    c+=sizeof(unsigned char)*offset;
    float value=0.0;
    memcpy(&value, c, sizeof(float));
    return value;
}

void ThermImporter::loadThermFile(const std::string &content)
{
    if (content.size()<2)
        return;

    // Check signature:
    // Position 0: 0x30
    // Position 1: 0Xbb
    if (readValueAtPosition(content, 0) != 0x30)
        return;
    if (readValueAtPosition(content, 1) != 0xbb)
        return;

    // Get header length.
    unsigned int header_length=headerLength(content);

    // Get materials list.
    std::string signature;
    signature.push_back((unsigned char)0x05);
    signature.push_back((unsigned char)0xbb);

    size_t p=content.find(signature);
    size_t max_p=content.find("FluxTagNodeData");
    while (p!=std::string::npos && p<max_p)
    {
        readMaterial(content, p);
        p=content.find(signature, p+1);
    }

    // Get volumes list.
    signature.clear();
    signature.push_back((unsigned char)0x0d);
    signature.push_back((unsigned char)0xbb);

    p=content.find(signature);
    while (p!=std::string::npos && p<max_p)
    {
        readPolygon(content, p);
        p=content.find(signature, p+1);
    }

    // Get boundary conditions list.
    // Signature must be search for after CPolyBC
    // has been closed with four 0xff.
    p=content.find("CPolyBC");
    if (p==std::string::npos)
        p=0;
    signature.clear();
    signature.push_back((unsigned char)0xff);
    signature.push_back((unsigned char)0xff);
    signature.push_back((unsigned char)0xff);
    signature.push_back((unsigned char)0xff);
    p=content.find(signature, p+1);

    signature.clear();
    signature.push_back((unsigned char)0x07);
    signature.push_back((unsigned char)0xbb);
    p=content.find(signature, p+1);
    while (p!=std::string::npos && p<max_p)
    {
        readBoundaryCondition(content, p);
        p=content.find(signature, p+1);
    }

    finalizePolygons();

    // Get surfaces.
    p=content.find("CPolyBC");
    if (p!=std::string::npos)
    {
        signature.clear();
        signature.push_back((unsigned char)0xff);
        signature.push_back((unsigned char)0xff);
        signature.push_back((unsigned char)0xff);
        signature.push_back((unsigned char)0xff);

        size_t p_end=content.find(signature, p+1);

        signature.clear();
        signature.push_back((unsigned char)0x07);
        signature.push_back((unsigned char)0xbb);
        p=content.find(signature, p+1);
        while (p!=std::string::npos && p<p_end)
        {
            readSurface(content, p);
            p=content.find(signature, p+1);
        }
    }
}

unsigned int ThermImporter::headerLength(const std::string &content) const
{
    unsigned int n_entries=32;
    unsigned int position=header_begin_offset;

    for (unsigned int i=0 ; i<n_entries ; i++)
    {
        // Read entry length.
        unsigned int length=readValueAtPosition(content, position);
        position+=length+1;        
    }
    return position-header_begin_offset-1;
}

void ThermImporter::readMaterial(const std::string &content, unsigned int offset)
{
    // Check for signature.
    // Position offset: 0x05
    // Position offset+1: 0Xbb
    if (readValueAtPosition(content, offset+0) != 0x05)
        return;
    if (readValueAtPosition(content, offset+1) != 0xbb)
        return;

    // Read material name length.
    unsigned int name_length=readValueAtPosition(content, offset+2);
    std::string name=readValue(content, offset+3, name_length);
    if (name.empty())
        return;

    // Read conductivity.
    float conductivity=readFloat(content, offset+name_length+3);

    // Read emissivity.
    float emissivity=readFloat(content, offset+name_length+11);

    // Read color.
    int red=readValueAtPosition(content, offset+name_length+15);
    int green=readValueAtPosition(content, offset+name_length+16);
    int blue=readValueAtPosition(content, offset+name_length+17);

    // Create material.
    Material *material=new Material;
    
    String s(name);
    std::string utf8_name=name;
    if (s.isLatin1())
        utf8_name=s.toUtf8();

    material->setName(utf8_name);
    material->setConductivity(conductivity, 0);
    material->setEmissivity(emissivity, 0);
    material->setColor(red, green, blue);

    // Test if duplicate already exists.
    Material *duplicate=Materials::instance()->duplicate(material);
    
    // If not duplicate: add material.
    if (!duplicate)
    {
        Materials::instance()->addMaterialToUserDb(material);
        _materialsMap[name]=material->id();
    }

    // Else: delete material.
    else
    {
        delete material;
        _materialsMap[name]=duplicate->id();
    }
}

void ThermImporter::readPolygon(const std::string &content, unsigned int offset)
{
    // Check for signature.
    // Position offset: 0x0d
    // Position offset+1: 0Xbb
    if (readValueAtPosition(content, offset+0) != 0x0d)
        return;
    if (readValueAtPosition(content, offset+1) != 0xbb)
        return;

    // Read material name length.
    unsigned int name_length=readValueAtPosition(content, offset+2);
    std::string name=readValue(content, offset+3, name_length);
    if (name.empty())
        return;

    // Read number of vertices.
    unsigned int n_vertices=readValueAtPosition(content, offset+name_length+7);
    if (n_vertices<3)
        return;

    std::vector<double> x_values;
    std::vector<double> y_values;

    // Read vertices.
    for (unsigned int i=0 ; i<n_vertices ; i++)
    {
        // Read coordinates.
        float x=readFloat(content, offset+name_length+9+i*8)/1000.0;
        float y=readFloat(content, offset+name_length+13+i*8)/1000.0;

        x_values.push_back(x);
        y_values.push_back(y);
    }

    // Remove uneeded points.
    std::vector<double> filtered_x_values;
    std::vector<double> filtered_y_values;

    for (unsigned int i=0 ; i<x_values.size() ; i++)
    {
        double x=x_values.at(i);
        double y=y_values.at(i);

        // Next point.
        double xp=x_values.at(0);
        double yp=y_values.at(0);
        if (i<x_values.size()-1)
        {
            xp=x_values.at(i+1);
            yp=y_values.at(i+1);
        }

        // Previous point.
        double xm=x_values.at(x_values.size()-1);
        double ym=y_values.at(y_values.size()-1);
        if (i>0)
        {
            xm=x_values.at(i-1);
            ym=y_values.at(i-1);
        }

        // If point is on segment defined by previous & next point: delete it.
        Line line;

        Point begin;
        begin.setX(xm);
        begin.setY(ym);
        line.setBegin(begin, 0);

        Point end;
        end.setX(xp);
        end.setY(yp);
        line.setEnd(end, 0);

        Point point;
        point.setX(x);
        point.setY(y);

        if (!GeometryTools::containsPoint(&line, point))
        {
            filtered_x_values.push_back(x);
            filtered_y_values.push_back(y);
        }
    }

    addPolygon(filtered_x_values, filtered_y_values, _materialsMap[name]);
}

void ThermImporter::readBoundaryCondition(const std::string &content, unsigned int offset)
{
    // Check for signature.
    // Position offset: 0x07
    // Position offset+1: 0Xbb
    if (readValueAtPosition(content, offset+0) != 0x07)
        return;
    if (readValueAtPosition(content, offset+1) != 0xbb)
        return;

    // Read boundary condition name length.
    unsigned int name_length=readValueAtPosition(content, offset+2);
    std::string name=readValue(content, offset+3, name_length);
    if (name.empty())
        return;

    // Read transmission coefficient.
    float h=readFloat(content, offset+name_length+5);
    if (std::fabs(h)<1.0e-12)
        return;

    // Read temperature.
    float t=readFloat(content, offset+name_length+13);

    // Read color.
    int red=readValueAtPosition(content, offset+name_length+17);
    int green=readValueAtPosition(content, offset+name_length+18);
    int blue=readValueAtPosition(content, offset+name_length+19);

    // Create boundary condition.
    BoundaryCondition *condition=new BoundaryCondition;

    String s(name);
    std::string utf8_name=name;
    if (s.isLatin1())
        utf8_name=s.toUtf8();

    condition->setName(utf8_name);
    condition->setResistance(1.0/h, 0);
    condition->setTemperature(t);
    condition->setColor(red, green, blue);

    // Test if duplicate already exists.
    BoundaryCondition *duplicate=BoundaryConditions::instance()->duplicate(condition);
    
    // If not duplicate: add boundary condition.
    if (!duplicate)
    {
        BoundaryConditions::instance()->addBcToUserDb(condition);
        _boundariesMap[name]=condition->id();
    }

    // Else: delete boundary condition.
    else
    {
        delete condition;
        _boundariesMap[name]=duplicate->id();
    }
}

void ThermImporter::readSurface(const std::string &content, unsigned int offset)
{
    // Check for signature.
    // Position offset: 0x07
    // Position offset+1: 0Xbb
    if (readValueAtPosition(content, offset+0) != 0x07)
        return;
    if (readValueAtPosition(content, offset+1) != 0xbb)
        return;

    // Read boundary condition name length.
    unsigned int name_length=readValueAtPosition(content, offset+2);
    std::string name=readValue(content, offset+3, name_length);
    if (name.empty())
        return;

    // Read edge type length.
    unsigned int edge_length=readValueAtPosition(content, offset+name_length+3);

    // Read begin vertex.
    float x0=readFloat(content, offset+name_length+edge_length+16)/1000.0;
    float y0=readFloat(content, offset+name_length+edge_length+20)/1000.0;

    // Read end vertex.
    float x1=readFloat(content, offset+name_length+edge_length+24)/1000.0;
    float y1=readFloat(content, offset+name_length+edge_length+28)/1000.0;

    addSurface(x0, y0, x1, y1, _boundariesMap[name]);
}

std::string ThermImporter::toThermFile(const std::vector<Volume*> &volumes, const std::vector<Surface*> &surfaces) const
{
    std::string content;

    // Header.
    unsigned char header[190]={0x30, 0xBB, 0x88, 0x13, 0x00, 0x00, 0x88, 0x13, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x43, 0x00, 0x00, 0x7A, 0xC3, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x43, 0x00, 0x00, 0x7A, 0xC3, 0xCD, 0xCC, 0xCC, 0x3D, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x43, 0x00, 0x00, 0x7A, 0xC3, 0x02, 0x00, 0x00, 0x00, 0x80, 0x3F, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x41, 0x01, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x80, 0x03, 0x11, 0x3A, 0x55, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x80, 0x0D, 0x11, 0x3A, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x7C, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x37, 0x2E, 0x33, 0x2E, 0x32, 0x2E, 0x30, 0x7C, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x37, 0x2E, 0x33, 0x2E, 0x32, 0x2E, 0x30, 0x7C, 0x7C, 0x04, 0x53, 0x69, 0x6C, 0x6C, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xBF, 0x01, 0x00, 0xFF, 0xFF, 0x02, 0x00, 0x0D, 0x00, 0x43, 0x50, 0x6F, 0x6C, 0x79, 0x4D, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61, 0x6C};
    for (unsigned int i=0 ; i<190 ; i++)
        content.push_back(header[i]);

    std::vector<Material*> materials;

    // Write all volumes.
    for (unsigned int i=0 ; i<volumes.size() ; i++)
    {
        Volume *volume=volumes.at(i);
        Material *material=volume->material();
        if (!material)
            continue;

        material->setId("Mohair");

        materials.push_back(material);
        std::string material_id=material->id();

        // Write volume header.
        content.push_back((unsigned char) 0x0d);
        content.push_back((unsigned char) 0xbb);

        // Write material id length.
        content.push_back(material_id.length());

        // Write material id.
        for (unsigned int j=0 ; j<material_id.length() ; j++)
            content.push_back(material_id.at(j));

        // Write some data.
        content.push_back((unsigned char) 0x01);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x01);
        content.push_back((unsigned char) 0x00);

        // Write number of vertices.
        content.push_back(volume->size());

        // Write some data.
        content.push_back((unsigned char) 0x00);

        // Write vertices.
        for (unsigned int j=0 ; j<volume->size() ; j++)
        {
            Path *path=volume->at(j);
            Point begin=path->begin();

            float x=begin.x()*1000.0;
            float y=begin.y()*1000.0;

            std::string s_x=writeFloat(x);
            std::string s_y=writeFloat(y);

            appendString(content, s_x);
            appendString(content, s_y);
        }

        // Write some data.
        unsigned char data[54]={0x50, 0x6E, 0x26, 0x04, 0x70, 0x6E, 0x26, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x74, 0x26, 0x04, 0x00, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x0D, 0x53, 0x68, 0x61, 0x64, 0x65, 0x4D, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61, 0x6C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
        for (unsigned int j=0 ; j<54 ; j++)
            content.push_back(data[j]);

        // Write some data for all volumes excepted last volume.
        if (i<volumes.size()-1)
        {
            content.push_back((unsigned char) 0x01);
            content.push_back((unsigned char) 0x80);
        }
    }

    // Write some data.
    content.push_back((unsigned char) 0xff);
    content.push_back((unsigned char) 0xff);
    content.push_back((unsigned char) 0x02);
    content.push_back((unsigned char) 0x00);
    content.push_back((unsigned char) 0x07);
    content.push_back((unsigned char) 0x00);
    content.push_back('C');
    content.push_back('P');
    content.push_back('o');
    content.push_back('l');
    content.push_back('y');
    content.push_back('B');
    content.push_back('C');

    std::vector<BoundaryCondition*> conditions;

    // Write surfaces data.
    for (unsigned int i=0 ; i<surfaces.size() ; i++)
    {
        Surface *surface=surfaces.at(i);

        // Surface boundary condition.
        BoundaryCondition *bc=surface->boundaryCondition();
        std::string bc_id="Adiabatic";
        if (bc)
        {
            conditions.push_back(bc);
            if (!bc->id().empty())
                bc_id=bc->id();
        }

        for (unsigned int j=0 ; j<surface->size() ; j++)
        {
            Line *line=surface->at(j);

            // Write surface header.
            content.push_back((unsigned char) 0x07);
            content.push_back((unsigned char) 0xbb);

            // Write boundary condition id length.
            content.push_back(bc_id.length());

            // Write boundary condition id.
            for (unsigned int j=0 ; j<bc_id.length() ; j++)
                content.push_back(bc_id.at(j));

            // Write some data.
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0x00);
            content.push_back((unsigned char) 0xd8);
            content.push_back((unsigned char) 0x74);
            content.push_back((unsigned char) 0x26);
            content.push_back((unsigned char) 0x04);
            content.push_back((unsigned char) 0x02);
            content.push_back((unsigned char) 0x00);

            // Write begin & end coordinates.
            Point begin=line->begin();
            float x=begin.x()*1000.0;
            float y=begin.y()*1000.0;

            std::string s_x=writeFloat(x);
            std::string s_y=writeFloat(y);

            appendString(content, s_x);
            appendString(content, s_y);

            Point end=line->end();
            x=end.x()*1000.0;
            y=end.y()*1000.0;

            s_x=writeFloat(x);
            s_y=writeFloat(y);

            appendString(content, s_x);
            appendString(content, s_y);

            // Write some data.
            unsigned char data[57]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x53, 0x68, 0x61, 0x64, 0x65, 0x4D, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61, 0x6C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80};
            for (unsigned int k=0 ; k<57 ; k++)
                content.push_back(data[k]);
        }
    }

    // Override last boundary condition.
    if (surfaces.size()>1)
    {
        content.at(content.size()-1)=0x00;
        content.at(content.size()-2)=0x00;
    }

    // Write some data.
    unsigned char data[53]={0x01, 0xBB, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0xA0, 0x40, 0x00, 0x00, 0xA0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xBB, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (unsigned int i=0 ; i<53 ; i++)
        content.push_back(data[i]);

    // Write all materials.
    ToolBox::uniquify(materials);
    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        Material *material=materials.at(i);

        // Write material header.
        content.push_back((unsigned char) 0x05);
        content.push_back((unsigned char) 0xbb);

        std::string material_id=material->id();

        material_id="Mohair";

        // Write material id length.
        content.push_back(material_id.length());

        // Write material id.
        for (unsigned int j=0 ; j<material_id.length() ; j++)
            content.push_back(material_id.at(j));

        // Write conductivity.
        float conductivity=material->conductivity();
        std::string s_conductivity=writeFloat(conductivity);
        appendString(content, s_conductivity);

        // Write some data.
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x3f);

        // Write emissivity.
        float emissivity=material->emissivity();
        std::string s_emissivity=writeFloat(emissivity);
        appendString(content, s_emissivity);

        // Write color.
        unsigned char red=material->red();
        unsigned char green=material->green();
        unsigned char blue=material->blue();

        content.push_back(red);
        content.push_back(green);
        content.push_back(blue);

        // Write some data.
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x04);
        content.push_back((unsigned char) 0x00);
    }

    // Write somme data.
    unsigned char data2[23]={0x05, 0xBB, 0x00, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x3F, 0x66, 0x66, 0x66, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (unsigned int i=0 ; i<23 ; i++)
        content.push_back(data2[i]);

    // Write boundary conditions.
    ToolBox::uniquify(conditions);

    // Add Adiabatic condition.
    BoundaryCondition bc;
    bc.setName("Adiabatic");
    bc.setResistance(1.0e+99, 0);
    bc.setTemperature(0.0);
    bc.setId("Adiabatic");
    conditions.push_back(&bc);

    for (unsigned int i=0 ; i<conditions.size() ; i++)
    {
        BoundaryCondition *condition=conditions.at(i);

        std::string id=condition->id();
        if (id.empty())
            continue;

        // Write condition header.
        content.push_back((unsigned char) 0x07);
        content.push_back((unsigned char) 0xbb);

        // Write condition id length.
        content.push_back(id.length());

        // Write condition id.
        for (unsigned int j=0 ; j<id.length() ; j++)
            content.push_back(id.at(j));

        // Write some data.
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);

        // Write transmission coefficient.
        float h=1.0/condition->resistance();
        std::string s_h=writeFloat(h);
        appendString(content, s_h);

        // Write some data.
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);
        content.push_back((unsigned char) 0x00);

        // Write temperature.
        float temperature=condition->temperature();
        std::string s_temperature=writeFloat(temperature);
        appendString(content, s_temperature);

        // Write some data.
        unsigned char data[48]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x48, 0x42};
        for (unsigned int i=0 ; i<48 ; i++)
            content.push_back(data[i]);
    }

    // Write some data.
    unsigned char data3[73]={0x07, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x48, 0x42, 0xFF, 0xFF, 0x01, 0xBB, 0x02, 0x00, 0x00, 0x00};
    for (unsigned int i=0 ; i<73 ; i++)
        content.push_back(data3[i]);

    return content;
}

std::string ThermImporter::writeFloat(float value) const
{
    unsigned char *data=(unsigned char *)malloc(sizeof(float));
    memset(data, 0, sizeof(float));
    memcpy(data, &value, sizeof(float));

    std::string content;
    for (unsigned int i=0 ; i<sizeof(float) ; i++)
        content.push_back(data[i]);

    free(data);
    return content;
}

void ThermImporter::appendString(std::string &content, const std::string &data) const
{
    for (unsigned int i=0 ; i<data.size() ; i++)
        content.push_back(data.at(i));
}
