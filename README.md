ShugoConsole
============

ShugoConsole allows the usage of the CryEngine's console in Aion.
NCSoft disabled access to the CryEngine's console in the western release of Aion.
Because it was the only way to modify some basic graphic parameters like the field of view
(which is necessary for large screens of multi-screen setup, i developped this program)

This utility can:
- enable the in-game console.
- automatically set the field of view.

Compatibility
-------------

ShugoConsole works with every version of Aion.
It doesn't use fixed offsets but scans the game's memory to locate the CryEngine's console variables.
When compiled in 64-bit mode, it works both with the 32-bit and 64-bit versions of the game.
When compiled in 32-bit mode, it only works for the 32-bit version.

Compiling
=========

The program can be compiled using QMake or Qt Creator.
You will need a Platform SDK with ntdll.lib.

Generating the MSI installer
----------------------------

An MSI installer can be generated using the WiX Toolset ( http://wixtoolset.org/ ).

Copy the generated executable to install/ShugoConsole
- ShugoConsole.exe

Then copy files from your Qt installation.

In install/ShugoConsole:
- bin/icudt51.dll
- bin/icuin51.dll
- bin/icuuc51.dll
- bin/libEGL.dll
- bin/libGLESv2.dll
- bin/Qt5Core.dll
- bin/Qt5Gui.dll
- bin/Qt5Widgets.dll

In install/ShugoConsole/plugins/platforms:
- plugins/platforms/qwindows.dll

Ensure any additional DLLs are included in the folder and referenced in ShugoConsole.wxs
(The included file is configured for Microsoft's Visual C++ 2012 compiler)

    >set PATH=C:\Program Files (x86)\WiW Toolset v3.7\bin;%PATH%
    >cd PATH\TO\install
    >candle.exe ShugoConsole.wxs
    >light.exe ShugoConsole.wixobj

You will also need to tweak the provided WiX script if you wish to generate a 32-bit installer.

