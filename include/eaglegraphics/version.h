#pragma once

namespace eaglegraphics
{
    struct Version
    {
        unsigned int major;
        unsigned int minor;
        unsigned int patch;
    };

    Version version() noexcept;
}