#pragma once

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

        void present() override;

    private:
        WindowHandle window_;
        bool initialized_ = false;
    };
}