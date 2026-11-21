// SPDX-License-Identifier: MIT
#include "GameChat.h"

#include <windows.h>

namespace
{
constexpr unsigned GameMode = 0xA8B238;
constexpr unsigned CurrentPlayer = 0xA83D4C;
constexpr unsigned Observer = 0xAC1198;
constexpr unsigned SessionNodes = 0xA8DA78;
constexpr unsigned NodeSpectatorFlag = 0x6B;
constexpr unsigned MessageList = 0xA8BC60;
constexpr unsigned EditBuffer = MessageList + 0x28;
constexpr unsigned EditInitialPos = MessageList + 0x2B4;
constexpr unsigned ChatTarget = 0xABCE18;
constexpr unsigned MaxNetworkText = 111;
constexpr int AllPlayers = 3;

enum class SessionMode : int
{
    Campaign = 0,
    Lan = 3,
    Internet = 4,
    Skirmish = 5,
};

// Enter the native chat packet setup at 0x55EDC8. The keyboard handler's
// epilogue pops EDI, ESI, EBP and EBX, releases its locals and returns.
__attribute__((naked, noinline)) void BroadcastChat()
{
    __asm__ volatile(
        "sub $0x8a0, %esp\n\t"
        "movl %edi, 0x00(%esp)\n\t"
        "movl %esi, 0x04(%esp)\n\t"
        "movl %ebp, 0x08(%esp)\n\t"
        "movl %ebx, 0x0c(%esp)\n\t"
        "movl $3, 0x10(%esp)\n\t" // Select the Unicode edit buffer.
        "xorl %ebx, %ebx\n\t"
        "movl $0x55edc8, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

void SendNetworkMessage(const wchar_t* message, unsigned length)
{
    const auto* nodes = *reinterpret_cast<const char* const* const*>(SessionNodes);
    if (!nodes || !nodes[0] || length > MaxNetworkText ||
        *reinterpret_cast<const UNALIGNED int*>(nodes[0] + NodeSpectatorFlag) == -1)
        return;

    auto* buffer = reinterpret_cast<volatile wchar_t*>(EditBuffer);
    auto& initialPos = *reinterpret_cast<volatile int*>(EditInitialPos);
    auto& target = *reinterpret_cast<volatile int*>(ChatTarget);
    const int savedPos = initialPos;
    const int savedTarget = target;
    wchar_t savedText[MaxNetworkText + 1];

    // Preserve the player's live chat input and temporarily select all-chat.
    for (unsigned i = 0; i < length; ++i)
    {
        savedText[i] = buffer[i];
        buffer[i] = message[i];
    }
    savedText[length] = buffer[length];
    buffer[length] = L'\0';
    initialPos = 0;
    target = AllPlayers;
    BroadcastChat();

    for (unsigned i = 0; i <= length; ++i)
        buffer[i] = savedText[i];
    initialPos = savedPos;
    target = savedTarget;
}

void ShowLocalMessage(const wchar_t* message)
{
    constexpr int MessageStyle = 0x4046;
    constexpr int DisplayFrames = 300;
    using AddMessage = void* (__thiscall*)(void*, const wchar_t*, int, const wchar_t*,
                                          int, int, int, bool);
    const auto addMessage = reinterpret_cast<AddMessage>(0x5D3BA0);
    addMessage(reinterpret_cast<void*>(MessageList), nullptr, 0, message,
               0, MessageStyle, DisplayFrames, true);
}
}

void SendMatchMessage(const wchar_t* message, unsigned length)
{
    const auto* player = *reinterpret_cast<const void* const*>(CurrentPlayer);
    if (!player || player == *reinterpret_cast<const void* const*>(Observer))
        return;

    const auto mode = *reinterpret_cast<const SessionMode*>(GameMode);
    if (mode == SessionMode::Lan || mode == SessionMode::Internet)
        SendNetworkMessage(message, length);
    else if (mode == SessionMode::Campaign || mode == SessionMode::Skirmish)
        ShowLocalMessage(message);
}
