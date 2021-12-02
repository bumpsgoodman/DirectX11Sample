#pragma once

#include <intrin.h>
#include <stdio.h>
#include <Windows.h>

#define AssertW(cond, msg) do                                                                                \
{                                                                                                            \
    if (!(cond))                                                                                             \
    {                                                                                                        \
        wchar_t buf[1024];                                                                                   \
        swprintf_s(buf, 1024, L"assertion fail: %s\nfile: %s\nline: %d\n", (msg), TEXT(__FILE__), __LINE__); \
        OutputDebugStringW(buf);                                                                             \
        MessageBoxW(nullptr, buf, L"Assertion failure", MB_ICONERROR | MB_OK);                               \
        __debugbreak();                                                                                      \
    }                                                                                                        \
} while (0)