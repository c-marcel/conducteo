**What is conducteo ?**

conducteo is a free software for thermal bridges computations according to EN 10211 standard.

**What license is applied ?**

conducteo is a free software released under the GPLv3 license.

**How to install conducteo ?**

Download & execute conducteo installers for Microsoft Windows, Mac OS X or Linux Debian.

**How to build conducteo ?**

Requirements:

- CMake
- c++ compiler
- Qt 6
- (Windows only) NSIS: Nullsoft Scriptable Install System

**Libraries dependencies**

conducteo uses the following dependencies:

- Qt 6
- Eigen
- tinyxml
- dxflib
- libzip

**Microsoft Windows build steps**

1. Download dependencies:
    - Eigen: https://gitlab.com/libeigen/eigen
    - dxflib: https://qcad.org/en/90-dxflib
    - tinyxml: https://sourceforge.net/projects/tinyxml
    - zlib: https://zlib.net/
    - libzip: https://github.com/nih-at/libzip
    - nlohmann-json: https://github.com/nlohmann/json
    
2. Extract dependencies

3. Build zlib with CMake

4. Build libzip with CMake and zlib outputs (ZLIB_LIBRARY and ZLIB_INCLUDE_DIR)

3. Set CMake variables:
    - EIGEN_ROOT_DIRECTORY: directory containing Eigen folder with headers
    - DXFLIB_ROOT_DIRECTORY: directory containing dxflib
    - TINYXML_ROOT_DIRECTORY: directory containing tinyxml
    - LIBZIP_ROOT_DIRECTORY: directory containing libzip Release outputs
    - JSON_ROOT_DIRECTORY: directory containing nlohmann-json (must contain nlohmann/json.hpp)
