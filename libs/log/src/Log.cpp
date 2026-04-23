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

#include <Log.h>
#include <cassert>
#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif

Log g_log_instance;

static std::string g_logLevelNames[] =
{
    "",
    "FATAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"
};

Log::Log() :
_outputFile( "conducteo.log" ),
_data( "" ),
_outputToFile( true ),
_outputToConsole( true ),
_functionNameColumnWidth( 50 ),
#ifdef DEBUG
_logLevel( LDEBUG )
#else
_logLevel( LWARNING )
#endif
{
}

void Log::setLogFileName( const std::string &fileName )
{
    _outputFile = fileName;
    if( _logLevel > LNONE )
    {
        // create a new file
#ifndef DISABLE_LOG
        std::ofstream outputFile2( _outputFile.c_str() );
        outputFile2.close();
#endif
    }
}

void Log::setLogLevel( const LogLevel level )
{
    _logLevel = level;
}

void Log::dump()
{
    std::ofstream outputFile2( _outputFile.c_str() );
    outputFile2 << _data;
    outputFile2.close();
}

std::string Log::getLogFileName() const
{
    return _outputFile;
}

void Log::add( LogLevel       Stype,
               std::string    Sfunc,
               std::string    Scontent,
               std::string    Sfile,
               int            Sline )
{
    add( g_logLevelNames[Stype], Sfunc, Scontent, Sfile, Sline );
}

std::string clampString( std::string &s, const unsigned int maxSize )
{
    unsigned int size = s.size();
    if( size <= maxSize )
        return s;
    if( maxSize <= 5 )
        return s;
    return s.substr(0, maxSize/2 - 2) + "..." + s.substr( size - maxSize / 2 + 1);
}

void Log::add( std::string    infoTypeName,
               std::string    Sfunc,
               std::string    Scontent,
               std::string    Sfile,
               int            Sline )
{
    // find info level
    int infoLevel;
    for( infoLevel = LNONE; infoLevel < LTRACE; infoLevel++ )
        if( infoTypeName == g_logLevelNames[ infoLevel ] )
            break;
    assert( infoLevel <= LTRACE );
    // Add date & time under the form DD-MM-YY HH:MM:SS
    time_t         rawtime;
    struct tm *    timeinfo;
    char           timeStr[80];
    time ( &rawtime );
    timeinfo = localtime( &rawtime );
    strftime( timeStr, 80, "%d%m%y:%H%M%S", timeinfo );
    // add in file and line info
    char fileInfos[256] = "";
    // disable file/line infos from INFO to TRACE levels
    if ( infoLevel <= LWARNING )
        snprintf( fileInfos, 256, "\n@ file: %s:%i", Sfile.c_str(), Sline );
    // Format: [dd-mm-yyyy hh:mm:ss] [WARNING] [thread_id] (Class::Function)    content   @filename:line
    char message[256];
    char content[256];
    snprintf( content, 256, "%%s %%-7s in [%%-8s]%%-%is\n", _functionNameColumnWidth );
    snprintf( message, 256, content, timeStr, infoTypeName.c_str(), clampString(Sfunc, _functionNameColumnWidth).c_str(), Scontent.c_str(), fileInfos );
    // "%s %-7s in [%-8s]%-80s: %-50s %s\n"
    // If LOG_LEVEL = 0: output in data string.
    if ( _logLevel == 0 )
        _data += message;
    else
        // output only message of level lower than _logLevel
        if( infoLevel <= _logLevel )
        {
            // output to the log file?
            if( _outputToFile )
            {
                std::ofstream outputFile2( _outputFile.c_str(), std::ios::app );
                outputFile2 << message;
                outputFile2.close();
            }
            // console mode.
            if ( _outputToConsole )
                std::cerr << message;
        }
    // Quit if fatal.
    if ( infoLevel == LFATAL )
        exit( 1 ); // XXX ugly!
}

void Log::data( const std::string & data )
{
    // output to the log file?
    if( _outputToFile )
    {
        std::ofstream outputFile2( _outputFile.c_str(), std::ios::app );
        outputFile2 << "--- begin data block ---" << std::endl;
        for ( int i = 0 ; i < ( int ) data.length() ; i++ )
            if ( data.at( i ) != '\0' )
                outputFile2 << data.at( i );
        outputFile2 << "--- end data block ---" << std::endl;
        outputFile2.close();
    }
    // console mode.
    if ( _outputToConsole )
        std::cerr << data;
}

Log::~Log()
{
}

std::string Log::printAsPointer( void * address )
{
    char buffer[50];
    sprintf ( buffer, "%p", address );
    return buffer;
}

void Log::enableConsoleOutputs( bool verbosity )
{
    _outputToConsole = verbosity;
}

void Log::setFunctionNameColumnSize( unsigned int width )
{
    _functionNameColumnWidth = width;
}

Log *Log::instance()
{
    return &g_log_instance;
}
