#pragma once

#include "types.h"

namespace eaglegraphics
{
    struct Size
    {
        Float32 width = 0.0f;
        Float32 height = 0.0f;

        constexpr Size() noexcept = default;

        constexpr Size(Float32 width, Float32 height) noexcept
            : width(width), height(height)
        {
        }

        constexpr bool operator==(const Size&) const noexcept = default;
    };
}