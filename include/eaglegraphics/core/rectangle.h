#pragma once

#include "point.h"
#include "size.h"

namespace eaglegraphics
{
    struct Rectangle
    {
        Point position;
        Size size;

        constexpr Rectangle() noexcept = default;

        constexpr Rectangle(Point position, Size size) noexcept
            : position(position), size(size)
        {
        }

        constexpr Rectangle(Float32 x, Float32 y, Float32 width, Float32 height) noexcept
            : position(x, y), size(width, height)
        {
        }

        constexpr Float32 left() const noexcept
        {
            return position.x;
        }

        constexpr Float32 top() const noexcept
        {
            return position.y;
        }

        constexpr Float32 right() const noexcept
        {
            return position.x + size.width;
        }

        constexpr Float32 bottom() const noexcept
        {
            return position.y + size.height;
        }

        constexpr bool contains(Point point) const noexcept
        {
            return point.x >= left() &&
                   point.x <= right() &&
                   point.y >= top() &&
                   point.y <= bottom();
        }
    };
}