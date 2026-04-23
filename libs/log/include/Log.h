// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2019, Clément MARCEL.
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

#ifndef _LOG_H_
#define _LOG_H_

#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <time.h>

class Log
{

public:

    enum LogLevel
    {
        LNONE    = 0,
        LFATAL   = 1,
        LERROR   = 2,
        LWARNING = 3,
        LINFO    = 4,
        LDEBUG   = 5,
        LTRACE   = 6
    };

    Log();
    virtual ~Log();

    void add( std::string    type,
                std::string    func,
                std::string    content,
                std::string    file,
                int            line);
    void add( LogLevel       type,
                std::string    func,
                std::string    content,
                std::string    file,
                int            line);

    void dump();
    std::string getLogFileName() const ;
    void setLogFileName( const std::string &fileName );
    void setLogLevel( const LogLevel level );

    void data( const std::string & data );
    std::string printAsPointer( void * address );

    void enableConsoleOutputs( bool verbosity );
    void setFunctionNameColumnSize( unsigned int width );

    static Log *instance();

private:

    std::string     _outputFile;        ///< Log file name
    std::string     _data;
    bool            _outputToFile;      ///< If true, will output messages to _outputFile.
    bool            _outputToConsole;   ///< If true, will output message to cout.
    LogLevel        _logLevel;          ///< Verbosity level; apply to file and console output.
    unsigned int    _functionNameColumnWidth;

};

#define GETLOG Log::instance()

#define DISABLE_LOG

// macros definitions.
#ifndef DISABLE_LOG
    #ifndef LOG_MACROS
        #define LOG_MACROS

        #define LOG_DUMP()      Log::instance()->dump()
        #define LOG_TRACE(x)   { std::stringstream oss; oss << x ; Log::instance()->add ( (std::string) "TRACE",   (std::string) __FUNCTION__, (std::string) oss.str(), (std::string) __FILE__, (int) __LINE__ );  }    
        #define LOG_DEBUG(x)   { std::stringstream oss; oss << x ; Log::instance()->add ( (std::string) "DEBUG",   (std::string) __FUNCTION__, (std::string) oss.str(), (std::string) __FILE__, (int) __LINE__ );  }    
        #define LOG_INFO(x)    { std::stringstream oss; oss << x ; Log::instance()->add ( (std::string) "INFO",    (std::string) __FUNCTION__, (std::string) oss.str(), (std::string) __FILE__, (int) __LINE__ );  }
        #define LOG_WARNING(x) { std::stringstream oss; oss << x ; Log::instance()->add ( (std::string) "WARNING", (std::string) __FUNCTION__, (std::string) oss.str(), (std::string) __FILE__, (int) __LINE__ );  }
        #define LOG_ERROR(x)   { std::stringstream oss; oss << x ; Log::instance()->add ( (std::string) "ERROR",   (std::string) __FUNCTION__, (std::string) oss.str(), (std::string) __FILE__, (int) __LINE__ );  }
        #define LOG_FATAL(x)   { std::stringstream oss; oss << x ; Log::instance()->add ( (std::string) "FATAL",   (std::string) __FUNCTION__, (std::string) oss.str(), (std::string) __FILE__, (int) __LINE__ );  }
        #define LOG_DATA(x)    { Log::getInstance()->data ( x );  }

    #endif
#else
    #ifndef LOG_MACROS
        #define LOG_MACROS

        #define LOG_DUMP()     {}
        #define LOG_TRACE(x)   {}
        #define LOG_DEBUG(x)   {}
        #define LOG_INFO(x)    {}
        #define LOG_WARNING(x) {}
        #define LOG_ERROR(x)   {}
        #define LOG_FATAL(x)   {}
        #define LOG_DATA(x)    {}

    #endif
#endif

#endif
