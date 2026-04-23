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

#include <actions/model/ImportVolumesFromXml.h>
#include <tools/ToolBox.h>
#include <model/Volume.h>
#include <model/Model.h>
#include <tinyxml.h>

ImportVolumesFromXml::ImportVolumesFromXml(Model *model, const std::string &xml, bool split):
    _model(model),
    _xml(xml),
    _split(split),
    _numberOfVolumes(0),
    _numberOfImportedVolumes(0)
{
}

ImportVolumesFromXml::~ImportVolumesFromXml()
{
}

void ImportVolumesFromXml::exec()
{
    storePhysics(_model);
    TiXmlDocument doc;
    doc.Parse(_xml.c_str());

    _volumes.clear();

    TiXmlElement *volumes=doc.RootElement();
    if (!volumes)
        return;

    TiXmlElement *volume=volumes->FirstChildElement("volume");
    _numberOfVolumes=0;
    _numberOfImportedVolumes=0;
    while (volume)
    {
        std::string volume_s=ToolBox::domToString(volume);
        std::map<std::string, std::string> dummy;
        Volume *v=Volume::createFromXml(volume_s, 0, dummy);
        
        if (v)
        {
            std::vector<Volume*> new_volumes;
            if (_split)
            {
                new_volumes=v->split();
                delete v;
            }
            else
                new_volumes.push_back(v);
            for (unsigned int j=0 ; j<new_volumes.size() ; j++)
            {
                _numberOfVolumes++;
                new_volumes.at(j)->close();

                if (new_volumes.at(j)->isClosed() && !new_volumes.at(j)->isSelfIntersected())
                {
                    _numberOfImportedVolumes++;
                    _model->addVolume(new_volumes.at(j));
                    _volumes.push_back(new_volumes.at(j)->id());
                }
                else
                {
                    delete new_volumes.at(j);
                }
            }

            // Select pasted objects.
            v->setSelected(true);
        }

        volume=volume->NextSiblingElement("volume");
    }
}

void ImportVolumesFromXml::undo()
{
    for (unsigned int i=0 ; i<_volumes.size() ; i++)
        _model->removeVolume(_volumes.at(i));
    _model->recreateSurfaces();
    restorePhysics(_model);
}

unsigned int ImportVolumesFromXml::numberOfVolumes() const
{
    return _numberOfVolumes;
}

unsigned int ImportVolumesFromXml::numberOfImportedVolumes() const
{
    return _numberOfImportedVolumes;
}

bool ImportVolumesFromXml::canceledAction() const
{
    return (_numberOfImportedVolumes==0);
}
