#ifndef REMOTEMEMORYLOOKUP_H
#define REMOTEMEMORYLOOKUP_H

#define WINDOW_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include "memorypattern.h"

class RemoteMemoryLookup
{
private:
    QByteArray buffer;
public:
    RemoteMemoryLookup(int _buffer_size = 131072);
    uint8_t* addressOfPattern(HANDLE hProcess, const MemoryPattern& pattern);
private:
    uint8_t* lookupPage(HANDLE hProcess, MEMORY_BASIC_INFORMATION* mem_info, const MemoryPattern& pattern);

};

#endif // REMOTEMEMORYLOOKUP_H
