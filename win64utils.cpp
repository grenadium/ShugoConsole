#include "win64utils.h"

typedef BOOL (WINAPI *IsWow64Process_t) (HANDLE,PBOOL);

bool IsProcess64(HANDLE hProcess)
{
    IsWow64Process_t _IsWow64Process;

    _IsWow64Process = (IsWow64Process_t)GetProcAddress(GetModuleHandleA("KERNEL32\0"), "IsWow64Process\0");

    if(_IsWow64Process != NULL)
    {
        if (sizeof(void *) == 8)
        {
            // This process is 64 bit hence 64-bit OS
            BOOL processisWOW64 = FALSE;
            _IsWow64Process(hProcess, &processisWOW64);
            return processisWOW64 == FALSE;
        }
        else
        {
            // This process is 32 bit
            HANDLE this_process = GetCurrentProcess();
            BOOL iamWOW64 = FALSE;
            _IsWow64Process(this_process, &iamWOW64);
            if(iamWOW64)
            {
                // This process is 32 bit on a 64-bit OS
                BOOL processisWOW64 = FALSE;
                _IsWow64Process(hProcess, &processisWOW64);
                return processisWOW64 == TRUE;
            }
            else
            {
                // This process is 32 bit on a 32-bit OS
                return false;
            }
        }
    }
    else
    {
        // OS is old ! This process is 32 bit on a 32-bit OS
        return false;
    }
}
