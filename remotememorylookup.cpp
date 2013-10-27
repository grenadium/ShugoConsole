#include "remotememorylookup.h"

RemoteMemoryLookup::RemoteMemoryLookup(int _buffer_size) : buffer(_buffer_size, 0) { }

uint8_t* RemoteMemoryLookup::lookupPage(HANDLE hProcess, MEMORY_BASIC_INFORMATION* mem_info, const MemoryPattern& pattern)
{
    uint8_t* begin = (uint8_t*)mem_info->BaseAddress;
    size_t remaining_bytes = mem_info->RegionSize;
    size_t bytes_read = 0;
    size_t bytes_to_read = remaining_bytes;

    if(bytes_to_read > buffer.size())
    {
        bytes_to_read = buffer.size();
    }
    while(ReadProcessMemory(hProcess, begin, buffer.data(), bytes_to_read, &bytes_read))
    {
        if(bytes_read == 0)
            break;

        size_t end = bytes_read - pattern.totalSize();

        for(size_t i = 0 ; i < end; i += 16)
        {
            if(pattern.match((BYTE*)(buffer.constData()+i)))
                return begin+i;
        }

        if(bytes_read >= remaining_bytes)
        {
            break;
        }
        else
        {
            remaining_bytes -= bytes_read;
            begin += bytes_read;

            bytes_to_read = remaining_bytes;
            if(bytes_to_read > buffer.size())
            {
                bytes_to_read = buffer.size();
            }
        }
    }

    return NULL;
}

uint8_t* RemoteMemoryLookup::addressOfPattern(HANDLE hProcess, const MemoryPattern& pattern)
{
    const char* read_address = 0;
    MEMORY_BASIC_INFORMATION mem_info = {0};
    uint8_t* ret = NULL;

    while(VirtualQueryEx(hProcess, read_address, &mem_info, sizeof(MEMORY_BASIC_INFORMATION)))
    {
        if (mem_info.Type == MEM_PRIVATE && mem_info.State == MEM_COMMIT)
        {
            if(mem_info.Protect == PAGE_READWRITE || mem_info.Protect == PAGE_EXECUTE_READWRITE)
            {
                ret = lookupPage(hProcess, &mem_info, pattern);

                if(ret)
                    break;
            }
        }

        const char* next_address = (const char*)mem_info.BaseAddress + mem_info.RegionSize;

        if(next_address > read_address)
        {
            read_address = next_address;
        }
        else
        {
            break;
        }
    }

    return ret;
}
