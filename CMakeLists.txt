cmake_minimum_required(VERSION 3.16)

project(conducteo-gpl)

# Eigen.
if (UNIX AND NOT APPLE)
    find_package(Eigen3 REQUIRED NO_MODULE)
else()
    set(EIGEN_ROOT_DIRECTORY "" CACHE PATH "Path to Eigen root directory")
    if (NOT EIGEN_ROOT_DIRECTORY)
        message(FATAL_ERROR "EIGEN_ROOT_DIRECTORY is not set. Please set it to your Eigen directory.")
    endif()
    
    include_directories(${EIGEN_ROOT_DIRECTORY})
endif()

# libdxf.
if (UNIX AND NOT APPLE)
    find_path(DXFLIB_INCLUDE_DIR NAMES dl_dxf.h PATH_SUFFIXES dxflib)
    find_library(DXFLIB_LIBRARY NAMES dxflib)

    if (NOT DXFLIB_INCLUDE_DIR OR NOT DXFLIB_LIBRARY)
        message(FATAL_ERROR "dxflib not found")
    endif()
else()
    set(DXFLIB_ROOT_DIRECTORY "" CACHE PATH "Path to dxflib root directory")
    
    if (NOT DXFLIB_ROOT_DIRECTORY)
        message(FATAL_ERROR "DXFLIB_ROOT_DIRECTORY is not set. Please set it to your dxflib directory.")
    endif()
    
    file(GLOB_RECURSE dxflib_src "${DXFLIB_ROOT_DIRECTORY}/src/*.cpp")
    set(DXFLIB_INCLUDE_DIR "${DXFLIB_ROOT_DIRECTORY}/src")
endif()

include_directories(${DXFLIB_INCLUDE_DIR})

# tinyxml.
if (WIN32)
    set(TINYXML_ROOT_DIRECTORY "" CACHE PATH "Path to tinyxml root directory")
    
    if (NOT TINYXML_ROOT_DIRECTORY)
        message(FATAL_ERROR "TINYXML_ROOT_DIRECTORY is not set. Please set it to your tinyxml directory.")
    endif()
    
    file(GLOB_RECURSE tinyxml_src "${TINYXML_ROOT_DIRECTORY}/tiny*.cpp")
    include_directories("${TINYXML_ROOT_DIRECTORY}")
endif()

# libzip.
if (WIN32)
    set(LIBZIP_ROOT_DIRECTORY "" CACHE PATH "Path to libzip root directory")
    
    if (NOT LIBZIP_ROOT_DIRECTORY)
        message(FATAL_ERROR "LIBZIP_ROOT_DIRECTORY is not set. Please set it to your libzip directory.")
    endif()
    
    include_directories("${LIBZIP_ROOT_DIRECTORY}")
    include_directories("${LIBZIP_ROOT_DIRECTORY}/lib")
    set(LIBZIP_LIBRARY "${LIBZIP_ROOT_DIRECTORY}/lib/Release/zip.lib")
endif()

# zlib.
if (WIN32)
    set(ZLIB_LIBRARY "" CACHE FILEPATH "Path to zlib library (.lib)")
    
    if (NOT ZLIB_LIBRARY)
        message(FATAL_ERROR "ZLIB_LIBRARY is not set. Please set it to your zlib library filepath.")
    endif()
endif()

# Software version.
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})

list(GET VERSION_LIST 0 CONDUCTEO_VERSION_MAJOR)
list(GET VERSION_LIST 1 CONDUCTEO_VERSION_MINOR)
list(GET VERSION_LIST 2 CONDUCTEO_VERSION_PATCH)

# Add preprocessing options for software version.
if (WIN32)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D CONDUCTEO_VERSION_MAJOR=${CONDUCTEO_VERSION_MAJOR} /D CONDUCTEO_VERSION_MINOR=${CONDUCTEO_VERSION_MINOR} /D CONDUCTEO_VERSION_PATCH=${CONDUCTEO_VERSION_PATCH} /D WIN32")
else ()
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONDUCTEO_VERSION_MAJOR=${CONDUCTEO_VERSION_MAJOR} -DCONDUCTEO_VERSION_MINOR=${CONDUCTEO_VERSION_MINOR} -DCONDUCTEO_VERSION_PATCH=${CONDUCTEO_VERSION_PATCH}")
endif()

set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui PrintSupport)

# conducteö headers.
include_directories("include")

# dependencies.
include_directories("libs/buttons-tab/include")
include_directories("libs/docx/include")
include_directories("libs/dxf/include")
include_directories("libs/linguist/include")
include_directories("libs/lists/include")
include_directories("libs/log/include")
include_directories("libs/local-data/include")
include_directories("libs/model-viewer/include")
include_directories("libs/mouse/include")
include_directories("libs/states/include")
include_directories("libs/string/include")
include_directories("libs/surface-results/include")
include_directories("libs/temperature/include")
include_directories("libs/volume-infos/include")
include_directories("libs/xml-parser/include")

set(rccs "resources/resources.qrc")

# conducteö software.
file(GLOB_RECURSE headers "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE headers_libs "${CMAKE_SOURCE_DIR}/libs/*.h")
file(GLOB_RECURSE sources "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE sources_c "${CMAKE_SOURCE_DIR}/libs/*.c")
file(GLOB_RECURSE libs "${CMAKE_SOURCE_DIR}/libs/*.cpp")

file(GLOB_RECURSE mocced_headers
    "${CMAKE_SOURCE_DIR}/include/ApplicationFactory.h"
    "${CMAKE_SOURCE_DIR}/include/MainMenu.h"
    "${CMAKE_SOURCE_DIR}/include/Preferences.h"
    "${CMAKE_SOURCE_DIR}/include/MainWindow.h"
    "${CMAKE_SOURCE_DIR}/include/computation/*.h"
    "${CMAKE_SOURCE_DIR}/include/importer/*.h"
    "${CMAKE_SOURCE_DIR}/include/reports/*.h"
    "${CMAKE_SOURCE_DIR}/include/tests/*.h"
    "${CMAKE_SOURCE_DIR}/libs/buttons-tab/*.h"
    "${CMAKE_SOURCE_DIR}/libs/lists/*.h"
    "${CMAKE_SOURCE_DIR}/libs/model-viewer/*.h"
    "${CMAKE_SOURCE_DIR}/libs/mouse/*.h"
    "${CMAKE_SOURCE_DIR}/libs/states/*.h"
    "${CMAKE_SOURCE_DIR}/libs/surface-results/*.h"
    "${CMAKE_SOURCE_DIR}/libs/temperature/*.h"
    "${CMAKE_SOURCE_DIR}/libs/volume-infos/*.h"
)

QT_WRAP_CPP(moc ${mocced_headers} )

# window icon (Win32 only).
if (WIN32)
    set(WIN32Resources "resources/windows/converter.rc")
endif()

# Mac OS X Core Foundation & Bundle creation.
if (APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    include(MacInstaller.cmake)
endif()

qt_add_executable(conducteo MANUAL_FINALIZATION MACOSX_BUNDLE ${MACOSXResources} ${libs} ${libs_win32} ${sources} ${sources_c} ${headers} ${headers_libs} ${moc} ${rccs} ${WIN32Resources} ${dxflib_src} ${tinyxml_src})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
    target_link_libraries(conducteo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport ${LIBZIP_LIBRARY} ${ZLIB_LIBRARY})
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(conducteo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport uuid zip tinyxml Eigen3::Eigen ${DXFLIB_LIBRARY})
endif()

if (APPLE)
    target_link_libraries(conducteo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport ${COREFOUNDATION_LIBRARY})
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(conducteo)
endif()

# Installers.

# Windows.
if (WIN32)
    include(WinInstaller.cmake)
endif()

# Linux Debian.
if (UNIX AND NOT APPLE)
    include(DebianInstaller.cmake)
endif()

