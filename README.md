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
The program was developped with Visual C++ 2012 and should compile with GCC (MinGW)

Generating the MSI installer
----------------------------

An MSI installer can be generated using the WiX Toolset ( http://wixtoolset.org/ ).

There are windows cmd scripts to automate the installer generation in the install subfolder.
Depending on your installation paths and compilers, you should tweak the scripts.
