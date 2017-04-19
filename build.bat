@echo off

REM  "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
set WERROR=

REM OLD set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi %WERROR% -W4  -wd4201 -wd4100 -wd4189 -wd4530 -wd4577 -wd4996 -FC -Z7 /I..\libs /I..\libs\glfw\include /I..\libs\openal\include /I..\libs\freetype\include

set CommonCompilerFlags=/MD -nologo -Od -Oi -W4 -Gm- -EHsc -wd4201 -wd4100 -wd4189 -wd4530 -wd4577 -wd4996 -wd4456 -FC -Z7 /I..\libs /I..\libs\glfw\include /I..\libs\openal\include /I..\libs\freetype\include /I..\libs\glad\include 
set CommonLinkerFlags= kernel32.lib user32.lib gdi32.lib winmm.lib opengl32.lib shell32.lib ..\libs\openal\libs\Win64\OpenAL32.lib ..\libs\glfw\lib-vc2015\glfw3.lib ..\libs\freetype\lib\freetype271d.lib 
set ExtraLinkerFlags=/NODEFAULTLIB:"LIBCMT" -incremental:no -opt:ref /ignore:4099

REM TODO - can we just build both with one exe?

IF NOT EXIST build mkdir build
pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL

echo Compilation started on: %time%
cl %CommonCompilerFlags% ..\src\game.cpp -Fmgame.map -LD /DLL /link %CommonLinkerFlags% %ExtraLinkerFlags% -PDB:game%random%.pdb -EXPORT:Update  
cl %CommonCompilerFlags% ..\src\main.cpp -Fmmain.map /link %ExtraLinkerFlags% %CommonLinkerFlags% glad.obj
echo Compilation finished on: %time%
popd
