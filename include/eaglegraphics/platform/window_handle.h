#pragma once

#include <cstdint>

namespace eaglegraphics
{
    struct WindowHandle
    {
        std::uintptr_t value = 0;

        constexpr WindowHandle() noexcept = default;

        constexpr explicit WindowHandle(std::uintptr_t native_handle) noexcept
            : value(native_handle)
        {
        }

        constexpr bool is_valid() const noexcept
        {
            return value != 0;
        }

        constexpr explicit operator bool() const noexcept
        {
            return is_valid();
        }

        constexpr bool operator==(const WindowHandle&) const noexcept = default;
    };
}