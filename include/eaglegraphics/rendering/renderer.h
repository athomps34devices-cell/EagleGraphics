#pragma once

#include <eaglegraphics/core/color.h>
#include <eaglegraphics/core/point.h>
#include <eaglegraphics/core/rectangle.h>

namespace eaglegraphics
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual void clear(const Color& color) = 0;

        virtual void draw_pixel(const Point& position, const Color& color) = 0;

        virtual void draw_line(
            const Point& start,
            const Point& end,
            const Color& color
        ) = 0;

        virtual void draw_rectangle(
            const Rectangle& rectangle,
            const Color& color
        ) = 0;

        virtual void fill_rectangle(
            const Rectangle& rectangle,
            const Color& color
        ) = 0;

        virtual void present() = 0;
    };
}