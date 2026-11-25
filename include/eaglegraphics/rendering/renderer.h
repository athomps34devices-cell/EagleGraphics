#pragma once

#include <span>
#include <string_view>

#include <eaglegraphics/core/color.h>
#include <eaglegraphics/core/font.h>
#include <eaglegraphics/core/point.h>
#include <eaglegraphics/core/rectangle.h>

namespace eaglegraphics
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual void clear(const Color& color) = 0;

        virtual void draw_pixel(
            const Point& position,
            const Color& color
        ) = 0;

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

        virtual void draw_ellipse(
            const Rectangle& bounds,
            const Color& color
        ) = 0;

        virtual void fill_ellipse(
            const Rectangle& bounds,
            const Color& color
        ) = 0;

        virtual void draw_circle(
            const Point& center,
            Float32 radius,
            const Color& color
        ) = 0;

        virtual void fill_circle(
            const Point& center,
            Float32 radius,
            const Color& color
        ) = 0;

        virtual void draw_polygon(
            std::span<const Point> points,
            const Color& color
        ) = 0;

        virtual void fill_polygon(
            std::span<const Point> points,
            const Color& color
        ) = 0;

        virtual void draw_text(
            const Point& position,
            std::string_view text,
            const Font& font,
            const Color& color
        ) = 0;

        virtual void present() = 0;
    };
}