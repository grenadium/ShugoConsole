@echo off

REM -- Environment variables definitions

set VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat
set QT32=C:\Qt\Qt5.1.0\5.1.0\msvc2012
set WIXTOOLSET=C:\Program Files (x86)\WiX Toolset v3.7\bin

set BUILD32=build.x86
set INSTALL32=ShugoConsole.x86

set OLDPATH=%PATH%

REM --
REM -- 32-bit compilation
REM --

set PATH=%QT32%\bin;%PATH%

rmdir /S /Q "%BUILD32%"
mkdir "%BUILD32%"
chdir "%BUILD32%"

call "%VCVARSALL%" x86
qmake -makefile "CONFIG+=release" ..\..\ShugoConsole.pro
nmake -f Makefile

set PATH=%OLDPATH%
cd ..

REM --
REM -- Copy files
REM --

rmdir /S /Q "%INSTALL32%"
mkdir "%INSTALL32%\plugins\platforms"

copy "%BUILD32%\release\ShugoConsole.exe" "%INSTALL32%"
copy "ShugoConsole\qt.conf" "%INSTALL32%"
copy "%QT32%\bin\icudt51.dll" "%INSTALL32%"
copy "%QT32%\bin\icuin51.dll" "%INSTALL32%"
copy "%QT32%\bin\icuuc51.dll" "%INSTALL32%"
copy "%QT32%\bin\libEGL.dll" "%INSTALL32%"
copy "%QT32%\bin\libGLESv2.dll" "%INSTALL32%"
copy "%QT32%\bin\Qt5Core.dll" "%INSTALL32%"
copy "%QT32%\bin\Qt5Gui.dll" "%INSTALL32%"
copy "%QT32%\bin\Qt5Widgets.dll" "%INSTALL32%"
copy "%QT32%\plugins\platforms\qwindows.dll" "%INSTALL32%\plugins\platforms"

REM --
REM -- Generate install files
REM --

set PATH=%WIXTOOLSET%;%PATH%

candle ShugoConsole.x86.wxs
light -ext WixUIExtension ShugoConsole.x86.wixobj
echo Y| cacls ShugoConsole.x86.msi /g Everyone:F

pause
