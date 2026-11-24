#pragma once

#include "types.h"

namespace eaglegraphics
{
    struct Color
    {
        UInt8 r = 0;
        UInt8 g = 0;
        UInt8 b = 0;
        UInt8 a = 255;

        constexpr Color() noexcept = default;

        constexpr Color(UInt8 red, UInt8 green, UInt8 blue) noexcept
            : r(red), g(green), b(blue), a(255)
        {
        }

        constexpr Color(UInt8 red, UInt8 green, UInt8 blue, UInt8 alpha) noexcept
            : r(red), g(green), b(blue), a(alpha)
        {
        }

        constexpr bool is_opaque() const noexcept
        {
            return a == 255;
        }

        constexpr bool is_transparent() const noexcept
        {
            return a == 0;
        }

        constexpr Color with_alpha(UInt8 alpha) const noexcept
        {
            return { r, g, b, alpha };
        }

        constexpr bool operator==(const Color&) const noexcept = default;
    };
}