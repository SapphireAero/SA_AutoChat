// SPDX-License-Identifier: MIT
#include "GameChat.h"

#include <windows.h>

namespace
{
constexpr unsigned CurrentFrame = 0xA8ED84;
constexpr unsigned ScenarioStarted = 0xA8ED5C;
constexpr int Interval = 2800;
constexpr wchar_t Message[] = L"我是羊了个羊加号！";

int lastFrame;
}

extern "C" __declspec(dllexport) DWORD __cdecl SA_AutoChat_OnFrame(void*)
{
    const int frame = *reinterpret_cast<const volatile int*>(CurrentFrame);
    if (frame == lastFrame) return 0;
    lastFrame = frame;

    if (frame > 0 && frame % Interval == 0 &&
        *reinterpret_cast<const volatile bool*>(ScenarioStarted))
        SendMatchMessage(Message, ARRAYSIZE(Message) - 1);

    return 0;
}
