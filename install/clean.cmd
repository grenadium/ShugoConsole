@echo off

rmdir /Q /S build.x86
rmdir /Q /S build.x64
rmdir /Q /S ShugoConsole.x86
rmdir /Q /S ShugoConsole.x64
del ShugoConsole.x86.wixobj
del ShugoConsole.x64.wixobj
del ShugoConsole.x86.wixpdb
del ShugoConsole.x64.wixpdb