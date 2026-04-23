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

#ifndef _ZIP_ARCHIVE_H
#define _ZIP_ARCHIVE_H

#include <vector>
#include <string>
#include <map>
#include <zip.h>

class ZipArchive
{

public:

    enum OpenFlags
    {
        CreateIfNotExist,
        ErrorIfAlreadyExist,
        CheckArchiveConsistency
    };

    ZipArchive();
    ZipArchive( const std::string & filename, OpenFlags flags = CheckArchiveConsistency );
    virtual ~ZipArchive();

    bool open( const std::string & filename, OpenFlags flags = CheckArchiveConsistency );

    std::vector<std::string> getFiles( const std::string & extension = "" ) const ;
    virtual std::string getFileContent( const std::string & filename ) const ;
    virtual void setFileContent( const std::string & filename, const std::string & content );

    int getError() const ;

    void close();
    bool save( const std::string & filename );

private:

    int getZipLibFlag( OpenFlags flag ) const ;
    void loadContent();

    zip                              * _archive;
    int                                _error;
    std::map<std::string, std::string> _content;

};

#endif
