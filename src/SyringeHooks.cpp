// SPDX-License-Identifier: MIT
#include <windows.h>

namespace
{
struct SyHookDecl
{
    unsigned address;
    unsigned size;
    const char* name;
    unsigned padding;
};

// Main loop tail, after the engine increments the logic frame.
const SyHookDecl hooks[] __attribute__((section(".syhks00"), used, aligned(16))) =
{
    {0x55DEA9, 6, "SA_AutoChat_OnFrame", 0},
};

struct SyringeHandshakeInfo
{
    int cbSize;
    int numHooks;
    unsigned checksum;
    DWORD exeFilesize;
    DWORD exeTimestamp;
    unsigned exeCRC;
    int messageCapacity;
    char* message;
};
}

extern "C" __declspec(dllexport) HRESULT __cdecl SyringeHandshake(SyringeHandshakeInfo* info)
{
    if (!info || info->cbSize < static_cast<int>(sizeof(SyringeHandshakeInfo))) return E_FAIL;
    const bool supported = info->exeFilesize == 4816864 && info->exeTimestamp == 0x3BDF544E &&
        info->exeCRC == 0x1B499086;
    if (info->message && info->messageCapacity > 0)
        lstrcpynA(info->message, supported ? "SA_AutoChat 1.0.0" :
            "SA_AutoChat requires the Mental Omega 3.3.6 game executable", info->messageCapacity);
    return supported ? S_OK : S_FALSE;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID)
{
    return TRUE;
}
