#ifndef WIN64UTILS_H
#define WIN64UTILS_H

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

bool IsProcess64(HANDLE hProcess);

#endif // WIN64UTILS_H
