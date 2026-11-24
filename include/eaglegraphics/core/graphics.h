#pragma once

#include "types.h"
#include "point.h"
#include "size.h"
#include "rectangle.h"

namespace eaglegraphics
{
    struct GraphicsInfo
    {
        UInt32 api_major = 0;
        UInt32 api_minor = 1;
        UInt32 api_patch = 0;
    };

    GraphicsInfo graphics_info() noexcept;
}