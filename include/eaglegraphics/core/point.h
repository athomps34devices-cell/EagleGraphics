#pragma once

#include "types.h"

namespace eaglegraphics
{
    struct Point
    {
        Float32 x = 0.0f;
        Float32 y = 0.0f;

        constexpr Point() noexcept = default;

        constexpr Point(Float32 x, Float32 y) noexcept
            : x(x), y(y)
        {
        }

        constexpr bool operator==(const Point&) const noexcept = default;
    };
}