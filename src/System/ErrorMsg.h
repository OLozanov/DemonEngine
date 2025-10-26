#pragma once

#include "System/Win32App.h"
#include <string>
#include <sstream>

#ifdef _DEBUG

#define ThrowIfFailed(hr) ThrowError(hr, ##__FILE__, ##__LINE__);

#else

#define ThrowIfFailed(expr) expr;

#endif

inline std::string to_hex(HRESULT hr)
{
    std::stringstream sstream;
    sstream << std::hex << hr;
    return sstream.str();
}

inline void ThrowError(HRESULT hr, const char* file, int line)
{
    if (FAILED(hr))
    {
        std::stringstream sstream;

        sstream << "File " << file << ", At line: " << line << ", HRESULT: " << std::hex << hr;
        //Win32App::ShowMessage(sstream.str().c_str(), "Runtime error\n");
        MessageBoxA(NULL, sstream.str().c_str(), "Runtime error\n", MB_OK);
        throw std::runtime_error(to_hex(hr));
    }
}