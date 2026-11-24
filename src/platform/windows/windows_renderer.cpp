#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <eaglegraphics/platform/windows/windows_renderer.h>

namespace eaglegraphics
{
    namespace
    {
        HWND get_native_window(WindowHandle handle) noexcept
        {
            return reinterpret_cast<HWND>(handle.value);
        }

        COLORREF to_native_color(const Color& color) noexcept
        {
            return RGB(color.r, color.g, color.b);
        }
    }

    WindowsRenderer::WindowsRenderer(WindowHandle window) noexcept
        : window_(window)
    {
    }

    WindowsRenderer::~WindowsRenderer()
    {
        shutdown();
    }

    bool WindowsRenderer::initialize() noexcept
    {
        if (!window_.is_valid())
        {
            return false;
        }

        if (!IsWindow(get_native_window(window_)))
        {
            return false;
        }

        initialized_ = true;
        return true;
    }

    void WindowsRenderer::shutdown() noexcept
    {
        initialized_ = false;
    }

    bool WindowsRenderer::is_initialized() const noexcept
    {
        return initialized_;
    }

    void WindowsRenderer::clear(const Color& color)
    {
        if (!initialized_)
        {
            return;
        }

        HWND window = get_native_window(window_);
        HDC device_context = GetDC(window);

        if (!device_context)
        {
            return;
        }

        RECT client_rectangle;

        if (GetClientRect(window, &client_rectangle))
        {
            HBRUSH brush = CreateSolidBrush(to_native_color(color));

            if (brush)
            {
                FillRect(device_context, &client_rectangle, brush);
                DeleteObject(brush);
            }
        }

        ReleaseDC(window, device_context);
    }

    void WindowsRenderer::draw_pixel(
        const Point& position,
        const Color& color
    )
    {
        if (!initialized_)
        {
            return;
        }

        HWND window = get_native_window(window_);
        HDC device_context = GetDC(window);

        if (!device_context)
        {
            return;
        }

        SetPixelV(
            device_context,
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            to_native_color(color)
        );

        ReleaseDC(window, device_context);
    }

    void WindowsRenderer::draw_line(
        const Point& start,
        const Point& end,
        const Color& color
    )
    {
        if (!initialized_)
        {
            return;
        }

        HWND window = get_native_window(window_);
        HDC device_context = GetDC(window);

        if (!device_context)
        {
            return;
        }

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (pen)
        {
            HGDIOBJ previous_pen = SelectObject(device_context, pen);

            MoveToEx(
                device_context,
                static_cast<int>(start.x),
                static_cast<int>(start.y),
                nullptr
            );

            LineTo(
                device_context,
                static_cast<int>(end.x),
                static_cast<int>(end.y)
            );

            SelectObject(device_context, previous_pen);
            DeleteObject(pen);
        }

        ReleaseDC(window, device_context);
    }

    void WindowsRenderer::draw_rectangle(
        const Rectangle& rectangle,
        const Color& color
    )
    {
        if (!initialized_)
        {
            return;
        }

        HWND window = get_native_window(window_);
        HDC device_context = GetDC(window);

        if (!device_context)
        {
            return;
        }

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (pen)
        {
            HGDIOBJ previous_pen = SelectObject(device_context, pen);
            HGDIOBJ previous_brush = SelectObject(
                device_context,
                GetStockObject(NULL_BRUSH)
            );

            ::Rectangle(
                device_context,
                static_cast<int>(rectangle.left()),
                static_cast<int>(rectangle.top()),
                static_cast<int>(rectangle.right()),
                static_cast<int>(rectangle.bottom())
            );

            SelectObject(device_context, previous_brush);
            SelectObject(device_context, previous_pen);

            DeleteObject(pen);
        }

        ReleaseDC(window, device_context);
    }

    void WindowsRenderer::fill_rectangle(
        const Rectangle& rectangle,
        const Color& color
    )
    {
        if (!initialized_)
        {
            return;
        }

        HWND window = get_native_window(window_);
        HDC device_context = GetDC(window);

        if (!device_context)
        {
            return;
        }

        RECT native_rectangle
        {
            static_cast<LONG>(rectangle.left()),
            static_cast<LONG>(rectangle.top()),
            static_cast<LONG>(rectangle.right()),
            static_cast<LONG>(rectangle.bottom())
        };

        HBRUSH brush = CreateSolidBrush(to_native_color(color));

        if (brush)
        {
            FillRect(device_context, &native_rectangle, brush);
            DeleteObject(brush);
        }

        ReleaseDC(window, device_context);
    }

    void WindowsRenderer::present()
    {
        // GDI draws directly to the window's device context.
        // Presentation is immediate; buffering will be added later.
    }
}