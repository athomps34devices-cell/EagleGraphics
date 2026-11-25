#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <span>
#include <string_view>

#include <eaglegraphics/platform/window_handle.h>
#include <eaglegraphics/rendering/renderer.h>

namespace eaglegraphics
{
    class WindowsRenderer : public Renderer
    {
    public:
        explicit WindowsRenderer(WindowHandle window) noexcept;

        ~WindowsRenderer() override;

        bool initialize() noexcept;
        void shutdown() noexcept;

        bool is_initialized() const noexcept;

        void clear(const Color& color) override;

        void draw_pixel(
            const Point& position,
            const Color& color
        ) override;

        void draw_line(
            const Point& start,
            const Point& end,
            const Color& color
        ) override;

        void draw_rectangle(
            const Rectangle& rectangle,
            const Color& color
        ) override;

        void fill_rectangle(
            const Rectangle& rectangle,
            const Color& color
        ) override;

        void draw_ellipse(
            const Rectangle& bounds,
            const Color& color
        ) override;

        void fill_ellipse(
            const Rectangle& bounds,
            const Color& color
        ) override;

        void draw_circle(
            const Point& center,
            Float32 radius,
            const Color& color
        ) override;

        void fill_circle(
            const Point& center,
            Float32 radius,
            const Color& color
        ) override;

        void draw_polygon(
            std::span<const Point> points,
            const Color& color
        ) override;

        void fill_polygon(
            std::span<const Point> points,
            const Color& color
        ) override;

        void draw_text(
            const Point& position,
            std::string_view text,
            const Font& font,
            const Color& color
        ) override;

        void present() override;

    private:
        bool resize_buffer(int width, int height) noexcept;
        void release_buffer() noexcept;

        WindowHandle window_;

        HDC buffer_dc_ = nullptr;
        HBITMAP buffer_bitmap_ = nullptr;
        HGDIOBJ previous_bitmap_ = nullptr;

        int buffer_width_ = 0;
        int buffer_height_ = 0;

        bool initialized_ = false;
    };
}