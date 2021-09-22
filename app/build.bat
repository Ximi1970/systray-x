@echo off 

if "%1" == "" (
  goto :usage
)
if "%2" == "" (
  goto :usage
)

set ARCH=
set SPEC=
set WIN=
if /I "%1"=="x86" (
  set ARCH=x86
  set WIN=win32
)
if /I "%1"=="x86_64" (
  set ARCH=x86_amd64
  set WIN=win64
)
if "%ARCH%" == "" (
  goto :usage
)

set QT_VER=
if /I "%2"=="5.14.2" (
  set QT_VER=5.14.2
  if "%ARCH%"=="x86" (
	set SPEC=msvc2017
  )
  if "%ARCH%"=="x86_amd64" (
    set SPEC=msvc2017_64
  )
)
if /I "%2"=="5.15.2" (
  set QT_VER=5.15.2
  if "%ARCH%"=="x86" (
	set SPEC=msvc2019
  )
  if "%ARCH%"=="x86_amd64" (
    set SPEC=msvc2019_64
  )
)
if "%QT_VER%" == "" (
  goto :usage
)

@REM Setup Visual Studio Express 2017

if "%QT_VER%"=="5.14.2" (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2017\WDExpress\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%
)

if "%QT_VER%"=="5.15.2" (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%
)

@REM Run qmake

C:\Qt\%QT_VER%\%SPEC%\bin\qmake ..\SysTray-X\SysTray-X.pro -spec win32-msvc
nmake

xcopy /Q /Y SysTray-X-app\release\SysTray-X.exe ..\dist\%WIN%\
xcopy /Q /Y ..\config\win32\SysTray_X.json.template ..\dist\%WIN%\
rename ..\dist\%WIN%\SysTray_X.json.template SysTray_X.json

C:\Qt\%QT_VER%\%SPEC%\bin\windeployqt.exe ..\dist\%WIN%\SysTray-X.exe

if "%WIN%" == "win64" (
  xcopy /Q /Y %SYSTEMROOT%\System32\msvcp140.dll ..\dist\%WIN%\
  xcopy /Q /Y %SYSTEMROOT%\System32\vcruntime140.dll ..\dist\%WIN%\
)

if "%WIN%" == "win32" (
  if exist %SYSTEMROOT%\SysWOW64\ (
    xcopy /Q /Y %SYSTEMROOT%\SysWOW64\msvcp140.dll ..\dist\%WIN%\
    xcopy /Q /Y %SYSTEMROOT%\SysWOW64\vcruntime140.dll ..\dist\%WIN%\
  )

  if not exist %SYSTEMROOT%\SysWOW64\ (
    xcopy /Q /Y %SYSTEMROOT%\System32\msvcp140.dll ..\dist\%WIN%\
    xcopy /Q /Y %SYSTEMROOT%\System32\vcruntime140.dll ..\dist\%WIN%\
  )
)

goto :end

:usage
  echo Usage: build.bat ^< x86 ^| x86_64 ^> ^< Qt version ^>

:end
