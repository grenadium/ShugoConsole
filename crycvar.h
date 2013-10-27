#ifndef CRYCVAR_H
#define CRYCVAR_H

#define WINDOW_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>

#define CVAR_MEM_SIZE_32 (28*16)
#define CVAR_MEM_NAME_32 5
#define CVAR_MEM_INT_32 160
#define CVAR_MEM_FLOAT_32 164
#define CVAR_MEM_STRING_32 168

#define CVAR_MEM_NAME_64 9
#define CVAR_MEM_INT_64 184
#define CVAR_MEM_FLOAT_64 188
#define CVAR_MEM_STRING_64 192

#define CVAR_STRING_SIZE 256
#define CVAR_MAX_SIZE (28*16)

class CryCVar
{
private:
    HANDLE hProcess;
    char* pmem;
    bool is64bit;

    struct {
        int int_value;
        int float_value;
        int string_value;
    } offsets;
public:
    CryCVar() : hProcess(0), pmem(NULL), is64bit(false) { setupoffsets(); }
    CryCVar(HANDLE _hProcess, char* _pmem, bool _is64bit) : hProcess(_hProcess), pmem(_pmem), is64bit(_is64bit) { setupoffsets(); }
    CryCVar(const CryCVar& o) : hProcess(o.hProcess), pmem(o.pmem), is64bit(o.is64bit) { setupoffsets(); }
    const CryCVar& operator=(const CryCVar& o) { hProcess = o.hProcess; pmem = o.pmem; is64bit = o.is64bit; setupoffsets(); return *this; }

    bool set(int i)
    {
        char buffer[CVAR_MAX_SIZE];
        memset(buffer,0,CVAR_MAX_SIZE);

        *(int*)(buffer+offsets.int_value) = i;
        *(float*)(buffer+offsets.float_value) = (float)i;
        int sz = sprintf_s((char*)(buffer+offsets.string_value),CVAR_STRING_SIZE,"%d",i);

        return TRUE == WriteProcessMemory(hProcess,pmem+offsets.int_value,
                                          buffer+offsets.int_value,
                                          sizeof(int)+sizeof(float)+sz+1,NULL);
    }

    bool set(float f)
    {
        char buffer[CVAR_MAX_SIZE];
        memset(buffer,0,CVAR_MAX_SIZE);

        *(int*)(buffer+offsets.int_value) = (int)f;
        *(float*)(buffer+offsets.float_value) = f;
        int sz = sprintf_s((char*)(buffer+offsets.string_value),CVAR_STRING_SIZE,"%f",f);

        return TRUE == WriteProcessMemory(hProcess,pmem+offsets.int_value,
                                          buffer+offsets.int_value,
                                          sizeof(int)+sizeof(float)+sz+1,NULL);
    }

    bool set(const char* s)
    {
        char buffer[CVAR_MAX_SIZE];
        memset(buffer,0,CVAR_MAX_SIZE);

        *(int*)(buffer+offsets.int_value) = atoi(s);
        *(float*)(buffer+offsets.float_value) = (float)atof(s);
        int sz = sprintf_s((char*)(buffer+offsets.string_value),CVAR_STRING_SIZE,"%s",s);

        return TRUE == WriteProcessMemory(hProcess,pmem+offsets.int_value,
                                          buffer+offsets.int_value,
                                          sizeof(int)+sizeof(float)+sz+1,NULL);
    }

    int intValue(bool *ok = NULL)
    {
        int i = 0;
        BOOL r = ReadProcessMemory(hProcess,pmem+offsets.int_value,(void*)&i,sizeof(int),NULL);
        if(ok)
            *ok = (r == TRUE);
        return i;
    }

    float floatValue(bool *ok = NULL)
    {
        float f = 0;
        BOOL r = ReadProcessMemory(hProcess,pmem+offsets.float_value,(void*)&f,sizeof(float),NULL);
        if(ok)
            *ok = (r == TRUE);
        return f;
    }

    bool valid() const { return hProcess != NULL && pmem != NULL; }

private:
    void setupoffsets()
    {
        if(is64bit)
        {
            offsets.int_value = CVAR_MEM_INT_64;
            offsets.float_value = CVAR_MEM_FLOAT_64;
            offsets.string_value = CVAR_MEM_STRING_64;
        }
        else
        {
            offsets.int_value = CVAR_MEM_INT_32;
            offsets.float_value = CVAR_MEM_FLOAT_32;
            offsets.string_value = CVAR_MEM_STRING_32;
        }
    }
};


#endif // CRYCVAR_H
