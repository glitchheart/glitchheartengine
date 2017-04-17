@echo off

set CMAKE="C:\Program Files\CMake\bin\cmake.exe"
set OPTIONS=-G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DGLFW_BUILD_TESTS=NO -DGLFW_BUILD_EXAMPLES=NO -DGLFW_BUILD_DOCS=NO
set GLFWDIR=

mkdir glfw-bin.WIN32\lib-vc2010
mkdir glfw-bin.WIN32\lib-vc2012
mkdir glfw-bin.WIN64\lib-vc2012
mkdir glfw-bin.WIN32\lib-vc2013
mkdir glfw-bin.WIN64\lib-vc2013
mkdir glfw-bin.WIN32\lib-vc2015
mkdir glfw-bin.WIN64\lib-vc2015

rem Visual C++ 2015 32-bit

mkdir build\vc2015-x86
cd    build\vc2015-x86

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
%CMAKE% %OPTIONS% -DBUILD_SHARED_LIBS=NO %GLFWDIR%
nmake
%CMAKE% %OPTIONS% -DBUILD_SHARED_LIBS=YES %GLFWDIR%
nmake

cd ..\..

copy build\vc2015-x86\src\glfw3.lib    glfw-bin.WIN32\lib-vc2015
copy build\vc2015-x86\src\glfw3dll.lib glfw-bin.WIN32\lib-vc2015
copy build\vc2015-x86\src\glfw3.dll    glfw-bin.WIN32\lib-vc2015

rem Visual C++ 2015 64-bit

mkdir build\vc2015-x64
cd    build\vc2015-x64

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
%CMAKE% %OPTIONS% -DBUILD_SHARED_LIBS=NO %GLFWDIR%
nmake
%CMAKE% %OPTIONS% -DBUILD_SHARED_LIBS=YES %GLFWDIR%
nmake

cd ..\..

copy build\vc2015-x64\src\glfw3.lib    glfw-bin.WIN64\lib-vc2015
copy build\vc2015-x64\src\glfw3dll.lib glfw-bin.WIN64\lib-vc2015
copy build\vc2015-x64\src\glfw3.dll    glfw-bin.WIN64\lib-vc2015

