#include <eaglegraphics/integration/graphics_context.h>

#include <memory>
#include <utility>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <eaglegraphics/platform/windows/windows_renderer.h>

#endif

namespace eaglegraphics
{
    struct GraphicsContext::Impl
    {
        WindowHandle window;

#if defined(_WIN32)
        std::unique_ptr<WindowsRenderer> renderer;
#endif

        explicit Impl(WindowHandle native_window)
            : window(native_window)
        {
        }
    };

    GraphicsContext::GraphicsContext(WindowHandle window)
        : impl_(std::make_unique<Impl>(window))
    {
    }

    GraphicsContext::~GraphicsContext()
    {
        shutdown();
    }

    GraphicsContext::GraphicsContext(GraphicsContext&&) noexcept = default;

    GraphicsContext& GraphicsContext::operator=(GraphicsContext&&) noexcept = default;

    bool GraphicsContext::initialize() noexcept
    {
        if (!impl_)
            return false;

    #if defined(_WIN32)
        if (impl_->renderer && impl_->renderer->is_initialized())
            return true;

        try
        {
            impl_->renderer = std::make_unique<WindowsRenderer>(
                impl_->window
            );

            if (!impl_->renderer->initialize())
            {
                impl_->renderer.reset();
                return false;
            }

            return true;
        }
        catch (...)
        {
            impl_->renderer.reset();
            return false;
        }
    #else
        return false;
    #endif
    }

    void GraphicsContext::shutdown() noexcept
    {
        if (!impl_)
            return;

#if defined(_WIN32)
        if (impl_->renderer)
        {
            impl_->renderer->shutdown();
            impl_->renderer.reset();
        }
#endif
    }

    bool GraphicsContext::is_initialized() const noexcept
    {
        if (!impl_)
            return false;

#if defined(_WIN32)
        return impl_->renderer && impl_->renderer->is_initialized();
#else
        return false;
#endif
    }

    Renderer* GraphicsContext::renderer() noexcept
    {
        if (!is_initialized())
            return nullptr;

#if defined(_WIN32)
        return impl_->renderer.get();
#else
        return nullptr;
#endif
    }

    const Renderer* GraphicsContext::renderer() const noexcept
    {
        if (!is_initialized())
            return nullptr;

#if defined(_WIN32)
        return impl_->renderer.get();
#else
        return nullptr;
#endif
    }

    void GraphicsContext::present() noexcept
    {
        if (!is_initialized())
            return;

#if defined(_WIN32)
        impl_->renderer->present();
#endif
    }

    bool GraphicsContext::resize() noexcept
    {
        if (!is_initialized())
            return false;

#if defined(_WIN32)
        return impl_->renderer->resize();
#else
        return false;
#endif
    }

    void GraphicsContext::paint() noexcept
    {
        if (!is_initialized())
            return;

#if defined(_WIN32)
        HWND window = reinterpret_cast<HWND>(impl_->window.value);

        if (!IsWindow(window))
            return;

        PAINTSTRUCT paint_info{};

        HDC target = BeginPaint(window, &paint_info);

        if (target)
            impl_->renderer->paint(target);

        EndPaint(window, &paint_info);
#endif
    }
}