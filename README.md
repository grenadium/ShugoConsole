ShugoConsole
============

ShugoConsole allows the modification of some variables of the CryEngine's console in Aion.
NCSoft disabled access to the CryEngine's console in the western release of Aion.
Because it was the only way to modify some basic graphic parameters like the field of view
(which is necessary for large screens of multi-screen setup), i developped this program.

This utility currently gives access to :
- Field of view
- Maximum camera distance
- Chat log
- FPS limit
- Triple buffering
- Anisotropic filtering

Compatibility
-------------

ShugoConsole works with every version of Aion.
It doesn't use fixed offsets but scans the game's memory to locate the CryEngine's console variables.
When compiled in 64-bit mode, it works both with the 32-bit and 64-bit versions of the game.
When compiled in 32-bit mode, it only works for the 32-bit version.

Compiling
=========

You will need a recent Qt version (5.x) and a compiler with C++11 support.

Visual C++
----------

Tested with Visual C++ 2013.
If you're using the Express edition of Visual Studio, you may need a Windows Platform SDK with ntdll.lib.

MinGW
-----

Tested with MinGW 4.8 (included in the Qt 5.2 installer from Digia)

Generating the MSI installer
----------------------------

A MSI installer can be generated using the WiX Toolset ( http://wixtoolset.org/ ).

There are windows cmd scripts to automate the installer generation in the install subfolder.
Depending on your Qt installation paths and compilers, you should tweak the scripts. 