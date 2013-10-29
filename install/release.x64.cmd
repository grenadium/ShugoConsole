@echo off

REM -- Environment variables definitions

set VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat
set QT64=C:\Qt\Qt5.1.1\5.1.1\msvc2012_64
set WIXTOOLSET=C:\Program Files (x86)\WiX Toolset v3.7\bin

set BUILD64=build.x64
set INSTALL64=ShugoConsole.x64

set OLDPATH=%PATH%

REM --
REM -- 64-bit compilation
REM --

set PATH=%QT64%\bin;%PATH%

rmdir /S /Q "%BUILD64%"
mkdir "%BUILD64%"
chdir "%BUILD64%"

call "%VCVARSALL%" x86_amd64
qmake -makefile "CONFIG+=release" ..\..\ShugoConsole.pro
nmake -f Makefile

set PATH=%OLDPATH%
cd ..

REM --
REM -- Copy files
REM --

rmdir /S /Q "%INSTALL64%"
mkdir "%INSTALL64%\plugins\platforms"

copy "%BUILD64%\release\ShugoConsole.exe" "%INSTALL64%"
copy "ShugoConsole\qt.conf" "%INSTALL64%"
copy "%QT64%\bin\icudt51.dll" "%INSTALL64%"
copy "%QT64%\bin\icuin51.dll" "%INSTALL64%"
copy "%QT64%\bin\icuuc51.dll" "%INSTALL64%"
copy "%QT64%\bin\libEGL.dll" "%INSTALL64%"
copy "%QT64%\bin\libGLESv2.dll" "%INSTALL64%"
copy "%QT64%\bin\Qt5Core.dll" "%INSTALL64%"
copy "%QT64%\bin\Qt5Gui.dll" "%INSTALL64%"
copy "%QT64%\bin\Qt5Widgets.dll" "%INSTALL64%"
copy "%QT64%\plugins\platforms\qwindows.dll" "%INSTALL64%\plugins\platforms"

REM --
REM -- Generate install files
REM --

set PATH=%WIXTOOLSET%;%PATH%

candle ShugoConsole.x64.wxs
light -ext WixUIExtension ShugoConsole.x64.wixobj
REM weird permission issue...
echo Y| cacls ShugoConsole.x64.msi /g Everyone:F

pause
