#pragma once

namespace eaglegraphics
{
    enum class Platform
    {
        Unknown,
        Windows,
        Linux,
        macOS
    };

    constexpr Platform current_platform() noexcept
    {
    #if defined(_WIN32)
        return Platform::Windows;
    #elif defined(__APPLE__)
        return Platform::macOS;
    #elif defined(__linux__)
        return Platform::Linux;
    #else
        return Platform::Unknown;
    #endif
    }
}