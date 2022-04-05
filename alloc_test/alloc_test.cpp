#include <iostream>
#include <windows.h>
#include <assert.h>

using namespace std;

//VirtualAllocs Param4
//https://docs.microsoft.com/en-us/windows/win32/memory/memory-protection-constants

class Entity
{
public:
    int health = 100;
};

static uint64_t image_base;
static uint32_t size_image;
static Entity* gLocalPlayer = nullptr;

LONG my_handler(_EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH; // not handler

    //#PF 会处理guard page
    //如果有必要需要第二次VirtualProtect

    cout << "exception address : " << hex << ExceptionInfo->ExceptionRecord->ExceptionAddress << endl;

    uint64_t base = 0;
    char fn[MAX_PATH]{};
    RtlPcToFileHeader(ExceptionInfo->ExceptionRecord->ExceptionAddress, (PVOID*)&base);
    GetModuleFileNameA((HMODULE)base, fn, 20);

    if (string(fn).find("SYSTEM32") != string::npos) //valid module
        return EXCEPTION_CONTINUE_EXECUTION;
    
    if (ExceptionInfo->ExceptionRecord->ExceptionAddress > (PVOID)image_base && ExceptionInfo->ExceptionRecord->ExceptionAddress < (PVOID)(image_base + size_image))
        return EXCEPTION_CONTINUE_EXECUTION; //handler exception

    return EXCEPTION_CONTINUE_SEARCH; //dont handler the exception
}

int main()
{
    DWORD op = 0;
    AddVectoredExceptionHandler(true,my_handler);

    image_base = (uint64_t)GetModuleHandleW(0);

    IMAGE_DOS_HEADER* idh = (decltype(idh))image_base;
    IMAGE_NT_HEADERS* inh = (decltype(inh))(idh->e_lfanew + image_base);
    size_image = inh->OptionalHeader.SizeOfImage;


    //gLocalPlayer = new Entity;
    gLocalPlayer = (Entity*)VirtualAlloc(0, sizeof(Entity), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    cout << "localp" << hex << gLocalPlayer << endl; 
    getchar();

    DebugBreak();
    if (!VirtualProtect((PVOID)gLocalPlayer, sizeof(Entity), PAGE_READONLY | PAGE_GUARD, &op))
    {
        cout << "vir p failed" << endl;
        return -1;
    }
    cout << "last error " << hex << GetLastError();
    DebugBreak();


    //first
    cout << gLocalPlayer->health << endl;
    DebugBreak();
    //secone
    cout << gLocalPlayer->health << endl;
    DebugBreak();

    cout << "wait\n";
    getchar();
    return 0;
}
