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

#include <tools/ZipArchive.h>
#include <sstream>
#include <fstream>

#ifdef __linux__
#include <string.h>
#endif

ZipArchive::ZipArchive():
    _archive(0),
    _error(0)
{
}

ZipArchive::ZipArchive(const std::string &filename, ZipArchive::OpenFlags flags):
    _archive(0),
    _error(0)
{
    open(filename, flags);
}

ZipArchive::~ZipArchive()
{
    close();
}

bool ZipArchive::open(const std::string &filename, ZipArchive::OpenFlags flags)
{
    // Close current archive.
    close();
    
    // Open provided archive.
    _archive=zip_open(filename.c_str(), getZipLibFlag(flags), &_error);
    if (!_archive)
        return false;
    
    loadContent();
    return true;
}

std::vector<std::string> ZipArchive::getFiles(const std::string &extension) const
{
    std::vector<std::string> files;
    if (_archive && _error==0)
    {
        zip_uint64_t nfiles=zip_get_num_entries(_archive, 0);
        for(int i=0 ; i<nfiles ; i++)
        {
            struct zip_stat fileinfo;
            zip_stat_index(_archive, i, 0, &fileinfo);
            const char *fname=zip_get_name(_archive, i, 0);
            if (fileinfo.size>0 && fileinfo.comp_size>0)
            {
                // Add file only if extension matches or no extension is provided.
                std::string s_name=fname;
                if (extension.empty() || s_name.find(extension)!=std::string::npos)
                    files.push_back(fname);
            }
        }
    }
    return files;
}

std::string ZipArchive::getFileContent(const std::string &filename) const
{
    std::map<std::string, std::string>::const_iterator it=_content.find(filename);
    if (it==_content.end())
        return "";
    return it->second;
}

void ZipArchive::setFileContent(const std::string &filename, const std::string &content)
{
    _content[filename]=content;
}

int ZipArchive::getError() const
{
    return _error;
}

void ZipArchive::close()
{
    _error=0;
    if (!_archive)
        return;
    zip_close(_archive);
    _archive=0;
}

bool ZipArchive::save(const std::string &filename)
{
    // Remove existing file.
    remove(filename.c_str());
    
    // Create temporary archive.
    zip *archive=zip_open(filename.c_str(), getZipLibFlag(CreateIfNotExist), &_error);
    if (!archive)
        return false;
    
    for(std::map<std::string, std::string>::iterator it=_content.begin() ; it!=_content.end() ; it++)
    {
        zip_source *source=zip_source_buffer(archive, it->second.c_str(), it->second.length(), 0);
        zip_add(archive, it->first.c_str(), source);
    }
    int code=zip_close(archive);
    if (code==-1)
        return false;
    return true;
}

void ZipArchive::loadContent()
{
    _content.clear();
    std::vector<std::string> files=getFiles();
    if (_archive && getError()==0)
    {
        for (unsigned int i=0 ; i<files.size() ; i++)
        {
            // Retrieve file informations.
            struct zip_stat fileinfo;
            zip_file *file=zip_fopen(_archive, files.at(i).c_str(), 0);
            zip_stat(_archive, files.at(i).c_str(), 0, &fileinfo);
            char *content=new char[(unsigned int) fileinfo.size];
            memset(content, 0, (unsigned int) fileinfo.size);
            zip_fread(file, content, fileinfo.size);
            
            std::string s_content;
            for (unsigned int j=0 ; j<(unsigned int) fileinfo.size ; j++)
                s_content+=content[j];
            delete[] content;
            zip_fclose( file );
            _content[files.at(i)]=s_content;
        }
    }
}

int ZipArchive::getZipLibFlag(ZipArchive::OpenFlags flag) const
{
    if (flag==ZipArchive::CreateIfNotExist)
        return ZIP_CREATE;
    if (flag==ZipArchive::ErrorIfAlreadyExist)
        return ZIP_EXCL;
    return ZIP_CHECKCONS;
}
