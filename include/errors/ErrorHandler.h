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

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <sstream>
#include <string>

#define setError(err, code) \
{ \
    if(err) \
    { \
        err->setCode(ErrorHandler::code); \
        std::ostringstream s; s<<__FILE__<<":"<<__LINE__<<"@"<<__FUNCTION__<<"[0x"<<std::hex<<this<<"]"; \
        err->setContext(s.str()); \
    } \
}

#define setStaticError(err, code) \
{ \
    if(err) \
    { \
        err->setCode(ErrorHandler::code); \
        std::ostringstream s; s<<__FILE__<<":"<<__LINE__<<"@"<<__FUNCTION__; \
        err->setContext(s.str()); \
    } \
}

#define setErrorWithDetails(err, code, details) \
{ \
    if (err) \
    { \
        setError(err, code); \
        err->setDetails(details); \
    } \
}

class ErrorHandler
{

public:

    enum ErrorCode
    {
        NoError=0,
        BadConductivity,
        BadResistance,
        BadRadius,
        BadCellWidth,
        BadCellHeight,
        BadModelFileRootElement,
        BadModelFileVersion,
        BadVolumeNode,
        BadArcNode,
        BadLineNode,
        BadMaterialNode,
        BadSurfaceNode,
        BadArcCenter,
        BadArcHorizontalRadius,
        BadArcVerticalRadius,
        BadArcBeginPoint,
        BadArcEndPoint,
        BadLineBeginPoint,
        BadLineEndPoint,
        BadMaterialId,
        BadMaterialName,
        BadMaterialColor,
        BadPathNode,
        UnknownPathType,
        BadPointNode,
        BadPointXValue,
        BadPointYValue,
        BadRotationNode,
        BadRotationCenter,
        BadRotationAngle,
        BadBcNode,
        BadBcId,
        BadBcName,
        BadBcResistance,
        BadBcTemperature,
        BadBcColor,
        SurfaceWithoutPath,
        BadEnvironmentName,
        BadEnvironmentColor,
        VolumeNotClosed,
        VolumeSelfIntersection,
        AbortByUser,
        BadImageNode,
        BadMaterialEmissivity,
        BadDxfImportNode,
        BadMaterialMu,
        BadBcRh
    };

    ErrorHandler();

    void setCode(ErrorCode code);
    void setContext(const std::string &context);
    void setDetails(const std::string &details);

    ErrorCode code() const;
    std::string context() const;
    std::string details() const;

    static std::string codeToString(ErrorCode code, const std::string &language = "fr");

private:

    ErrorCode       _code;
    std::string     _context;
    std::string     _details;

};

#endif
