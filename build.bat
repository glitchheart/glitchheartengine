@echo off

set Libdir=.\libs
set glfw_inc=%Libdir%\glfw\include
set glfw_lib=%Libdir%\glfw\lib-vc2015\

REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
set INCLUDE=%INCLUDE%
set WERROR=

REM OLD set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi %WERROR% -W4  -wd4201 -wd4100 -wd4189 -wd4530 -wd4577 -wd4996 -FC -Z7 /I..\libs /I..\libs\glfw\include /I..\libs\openal\include /I..\libs\freetype\include

set CommonCompilerFlags=/MT -nologo -Od -Oi -W4 -Gm- -EHsc -wd4201 -wd4100 -wd4189 -wd4530 -wd4577 -wd4996 -FC -Z7 /I..\libs /I..\libs\glfw\include /I..\libs\openal\include /I..\libs\freetype\include /I..\libs\glad\
set CommonLinkerFlags= /ignore:4217 /ignore:4049 /ignore:4099 -incremental:no -opt:ref /LIBPATH:..\libs kernel32.lib user32.lib gdi32.lib winmm.lib glad.obj opengl32.lib ..\libs\openal\libs\Win64\OpenAL32.lib glfw\lib-vc2015\glfw3.lib ..\libs\freetype\lib\freetype271d.lib

REM TODO - can we just build both with one exe?

IF NOT EXIST build mkdir build
pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\src\game.cpp -Fmgame.map -LD /DLL /link -incremental:no -opt:ref -PDB:game%random%.pdb -EXPORT:Update %CommonLinkerFlags%
cl %CommonCompilerFlags% ..\src\main.cpp -Fmmain.map /link %CommonLinkerFlags% 
popd
