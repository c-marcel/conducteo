**What is conducteö ?**

conducteö is a free software for thermal bridges computations according to EN 10211 standard.

**What license is applied ?**

conducteö is a free software released under the GPLv3 license.

**How to install conducteö ?**

Download & execute conducteö installers for Microsoft Windows, Mac OS X or Linux Debian.

**How to build conducteö ?**

Requirements:

- CMake
- c++ compiler
- Qt 6
- (Windows only) NSIS: Nullsoft Scriptable Install System

**Libraries dependencies**

conducteö uses the following dependencies:

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
    
2. Extract dependencies

3. Set CMake variables:
    - EIGEN_ROOT_DIRECTORY: directory containing Eigen folder with headers
    - DXFLIB_ROOT_DIRECTORY: directory containing dxflib
    - TINYXML_ROOT_DIRECTORY: directory containing tinyxml
