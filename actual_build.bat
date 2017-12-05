@echo off

set WERROR=
set DEBUG=-DGLITCH_DEBUG=1
set GLM=
set PRP=
set WIGNORED=-wd4201 -wd4505 -wd4100 -wd4996 -wd4456 -wd4127 -wd4582 -wd4587 -wd4820 -wd4061 -wd4710 -wd4191 -wd4623 -wd4625 -wd5026 

WHERE cl
IF %ERRORLEVEL% NEQ 0 call %VCVARSALL% x64

echo %cd%
set CommonCompilerFlags=/MD -nologo -Od -Oi -Wall -Gm- -EHsc -FC -Z7 %PRP% %WIGNORED% %DEBUG% %GLM% /I..\libs /I..\libs\glfw\include /I..\libs\freetype\include /I..\libs\glad\include /I ..\libs\fmod\include
set CommonLinkerFlags= kernel32.lib user32.lib gdi32.lib winmm.lib opengl32.lib shell32.lib  ..\libs\glfw\lib-vc2015\glfw3.lib ..\libs\glad\glad.obj ..\libs\freetype\lib\freetype271d.lib ..\libs\fmod\lib\fmod64_vc.lib
set ExtraLinkerFlags=/NODEFAULTLIB:"LIBCMT" -incremental:no -opt:ref /ignore:4099

IF NOT EXIST build mkdir build
pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL

echo Compilation started on: %time%
REM cl %CommonCompilerFlags% ..\src\game.cpp  -LD /DLL /link -incremental:no -PDB:game%random%.pdb -EXPORT:Update  
cl %CommonCompilerFlags% ..\src\win32_main.cpp -Femain  /link %ExtraLinkerFlags% %CommonLinkerFlags%
echo Compilation finished on: %time%
popd