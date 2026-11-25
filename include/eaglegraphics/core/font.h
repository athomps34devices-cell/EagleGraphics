#pragma once

#include <string>

#include <eaglegraphics/core/types.h>

namespace eaglegraphics
{
    struct Font
    {
        std::string family = "Arial";
        Float32 size = 16.0f;
        bool bold = false;
        bool italic = false;

        Font() = default;

        explicit Font(
            std::string font_family,
            Float32 font_size = 16.0f,
            bool is_bold = false,
            bool is_italic = false
        )
            : family(std::move(font_family)),
              size(font_size),
              bold(is_bold),
              italic(is_italic)
        {
        }

        bool operator==(const Font&) const = default;
    };
}