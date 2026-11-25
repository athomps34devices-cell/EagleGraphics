#pragma once

#include <memory>

#include <eaglegraphics/platform/window_handle.h>
#include <eaglegraphics/rendering/renderer.h>

namespace eaglegraphics
{
    class GraphicsContext
    {
    public:
        explicit GraphicsContext(WindowHandle window);
        ~GraphicsContext();

        GraphicsContext(const GraphicsContext&) = delete;
        GraphicsContext& operator=(const GraphicsContext&) = delete;

        GraphicsContext(GraphicsContext&&) noexcept;
        GraphicsContext& operator=(GraphicsContext&&) noexcept;

        bool initialize() noexcept;
        void shutdown() noexcept;

        bool is_initialized() const noexcept;

        Renderer* renderer() noexcept;
        const Renderer* renderer() const noexcept;

        void present() noexcept;
        bool resize() noexcept;
        void paint() noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}