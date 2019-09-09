@echo off

set EXECUTABLE_NAME=altered
set WERROR=
set DEBUG=1
set GLM=
set PRP=
set WIGNORED=-wd5039 -wd4201 -wd4505 -wd4100 -wd4996 -wd4456 -wd4582 -wd4587 -wd4820 -wd4061 -wd4710 -wd4191 -wd4623 -wd4625 -wd5026 -wd4668 -wd4312 -wd5045 -wd4577 -wd4711 -wd4514
set OPT=-Od
set ASM=/FA
set AnalyticsPath=..\libs\gameanalytics
set AnalyticsSrcPath=%AnalyticsPath%\source
set AnalyticsDeps=%AnalyticsSrcPath%\dependencies

WHERE cl
IF %ERRORLEVEL% NEQ 0 call %VCVARSALL% x64

echo %cd%
set CommonCompilerFlags
=/MD -std:c++latest -fp:fast -fp:except- -nologo %OPT% -Oi -W4 -Gm- -GR- -EHa -FC -Z7 -Fe%EXECUTABLE_NAME% %PRP% %WIGNORED% -DDEBUG=%DEBUG% %GLM% /I..\libs /I%AnalyticsSrcPath%\gameanalytics\ /I..\libs\glfw\include /I..\libs\libcurl\include  /I..\libs\glad\include /I ..\libs\fmod\include /I..\libs\vulkan\Include
set CommonLinkerFlags=Comdlg32.lib Ole32.lib kernel32.lib user32.lib gdi32.lib winmm.lib opengl32.lib shell32.lib Ws2_32.lib Wldap32.lib Crypt32.lib Normaliz.lib Advapi32.lib ..\libs\libcurl\win\libcurl_a.lib ..\libs\glfw\lib-vc2015\glfw3.lib ..\libs\glad\glad.obj ..\libs\fmod\lib\fmod64_vc.lib %AnalyticsDeps%\curl\lib\win64\vc140_x64_release\libcurl.lib %AnalyticsDeps%\openssl\1.0.2h\libs\win64\vc140_x64_release\libeay32.lib %AnalyticsDeps%\openssl\1.0.2h\libs\win64\vc140_x64_release\ssleay32.lib %AnalyticsPath%\win64-vc140-static\Release\GameAnalytics.lib
set ExtraLinkerFlags=/NODEFAULTLIB:"LIBCMT" -incremental:no -opt:ref /ignore:4099

IF %DEBUG% NEQ 1 set Console=/SUBSYSTEM:windows /ENTRY:mainCRTStartup

IF NOT EXIST build mkdir build
pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL

echo Compilation started on: %time%
cl %CommonCompilerFlags% ..\src\main.cpp /Fealtered /DCURL_STATICLIB /link %ExtraLinkerFlags% %CommonLinkerFlags%
echo Compilation finished on: %time%
popd
