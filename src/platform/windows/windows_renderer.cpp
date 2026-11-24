#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <vector>
#include <cstdint>

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

        RECT to_native_rectangle(const Rectangle& rectangle) noexcept
        {
            return RECT
            {
                static_cast<LONG>(rectangle.left()),
                static_cast<LONG>(rectangle.top()),
                static_cast<LONG>(rectangle.right()),
                static_cast<LONG>(rectangle.bottom())
            };
        }

        POINT to_native_point(const Point& point) noexcept
        {
            return POINT
            {
                static_cast<LONG>(point.x),
                static_cast<LONG>(point.y)
            };
        }

        bool create_pen_and_brush(
            HDC device_context,
            const Color& color,
            HPEN& pen,
            HBRUSH& brush,
            HGDIOBJ& previous_pen,
            HGDIOBJ& previous_brush
        )
        {
            pen = CreatePen(PS_SOLID, 1, to_native_color(color));
            brush = CreateSolidBrush(to_native_color(color));

            if (!pen || !brush)
            {
                if (pen)
                    DeleteObject(pen);

                if (brush)
                    DeleteObject(brush);

                return false;
            }

            previous_pen = SelectObject(device_context, pen);
            previous_brush = SelectObject(device_context, brush);

            return true;
        }

        void restore_pen_and_brush(
            HDC device_context,
            HPEN pen,
            HBRUSH brush,
            HGDIOBJ previous_pen,
            HGDIOBJ previous_brush
        )
        {
            SelectObject(device_context, previous_brush);
            SelectObject(device_context, previous_pen);

            DeleteObject(brush);
            DeleteObject(pen);
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
            return false;

        if (!IsWindow(get_native_window(window_)))
            return false;

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
            return;

        HWND window = get_native_window(window_);
        HDC dc = GetDC(window);

        if (!dc)
            return;

        RECT bounds;

        if (GetClientRect(window, &bounds))
        {
            HBRUSH brush = CreateSolidBrush(to_native_color(color));

            if (brush)
            {
                FillRect(dc, &bounds, brush);
                DeleteObject(brush);
            }
        }

        ReleaseDC(window, dc);
    }

    void WindowsRenderer::draw_pixel(
        const Point& position,
        const Color& color
    )
    {
        if (!initialized_)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        SetPixelV(
            dc,
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            to_native_color(color)
        );

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::draw_line(
        const Point& start,
        const Point& end,
        const Color& color
    )
    {
        if (!initialized_)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (pen)
        {
            HGDIOBJ previous = SelectObject(dc, pen);

            MoveToEx(
                dc,
                static_cast<int>(start.x),
                static_cast<int>(start.y),
                nullptr
            );

            LineTo(
                dc,
                static_cast<int>(end.x),
                static_cast<int>(end.y)
            );

            SelectObject(dc, previous);
            DeleteObject(pen);
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::draw_rectangle(
        const Rectangle& rectangle,
        const Color& color
    )
    {
        if (!initialized_)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (pen)
        {
            HGDIOBJ previous_pen = SelectObject(dc, pen);
            HGDIOBJ previous_brush = SelectObject(
                dc,
                GetStockObject(NULL_BRUSH)
            );

            ::Rectangle(
                dc,
                static_cast<int>(rectangle.left()),
                static_cast<int>(rectangle.top()),
                static_cast<int>(rectangle.right()),
                static_cast<int>(rectangle.bottom())
            );

            SelectObject(dc, previous_brush);
            SelectObject(dc, previous_pen);

            DeleteObject(pen);
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::fill_rectangle(
        const Rectangle& rectangle,
        const Color& color
    )
    {
        if (!initialized_)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        RECT bounds = to_native_rectangle(rectangle);
        HBRUSH brush = CreateSolidBrush(to_native_color(color));

        if (brush)
        {
            FillRect(dc, &bounds, brush);
            DeleteObject(brush);
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::draw_ellipse(
        const Rectangle& bounds,
        const Color& color
    )
    {
        if (!initialized_)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (pen)
        {
            HGDIOBJ previous_pen = SelectObject(dc, pen);
            HGDIOBJ previous_brush = SelectObject(
                dc,
                GetStockObject(NULL_BRUSH)
            );

            RECT native_bounds = to_native_rectangle(bounds);

            ::Ellipse(
                dc,
                native_bounds.left,
                native_bounds.top,
                native_bounds.right,
                native_bounds.bottom
            );

            SelectObject(dc, previous_brush);
            SelectObject(dc, previous_pen);

            DeleteObject(pen);
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::fill_ellipse(
        const Rectangle& bounds,
        const Color& color
    )
    {
        if (!initialized_)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        HPEN pen;
        HBRUSH brush;
        HGDIOBJ previous_pen;
        HGDIOBJ previous_brush;

        if (create_pen_and_brush(
            dc,
            color,
            pen,
            brush,
            previous_pen,
            previous_brush
        ))
        {
            RECT native_bounds = to_native_rectangle(bounds);

            ::Ellipse(
                dc,
                native_bounds.left,
                native_bounds.top,
                native_bounds.right,
                native_bounds.bottom
            );

            restore_pen_and_brush(
                dc,
                pen,
                brush,
                previous_pen,
                previous_brush
            );
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::draw_circle(
        const Point& center,
        Float32 radius,
        const Color& color
    )
    {
        if (radius < 0)
            return;

        Rectangle bounds(
            Point(center.x - radius, center.y - radius),
            Size(radius * 2, radius * 2)
        );

        draw_ellipse(bounds, color);
    }

    void WindowsRenderer::fill_circle(
        const Point& center,
        Float32 radius,
        const Color& color
    )
    {
        if (radius < 0)
            return;

        Rectangle bounds(
            Point(center.x - radius, center.y - radius),
            Size(radius * 2, radius * 2)
        );

        fill_ellipse(bounds, color);
    }

    void WindowsRenderer::draw_polygon(
        std::span<const Point> points,
        const Color& color
    )
    {
        if (!initialized_ || points.size() < 3)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        std::vector<POINT> native_points;
        native_points.reserve(points.size());

        for (const Point& point : points)
            native_points.push_back(to_native_point(point));

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (pen)
        {
            HGDIOBJ previous_pen = SelectObject(dc, pen);
            HGDIOBJ previous_brush = SelectObject(
                dc,
                GetStockObject(NULL_BRUSH)
            );

            ::Polygon(
                dc,
                native_points.data(),
                static_cast<int>(native_points.size())
            );

            SelectObject(dc, previous_brush);
            SelectObject(dc, previous_pen);

            DeleteObject(pen);
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::fill_polygon(
        std::span<const Point> points,
        const Color& color
    )
    {
        if (!initialized_ || points.size() < 3)
            return;

        HDC dc = GetDC(get_native_window(window_));

        if (!dc)
            return;

        std::vector<POINT> native_points;
        native_points.reserve(points.size());

        for (const Point& point : points)
            native_points.push_back(to_native_point(point));

        HPEN pen;
        HBRUSH brush;
        HGDIOBJ previous_pen;
        HGDIOBJ previous_brush;

        if (create_pen_and_brush(
            dc,
            color,
            pen,
            brush,
            previous_pen,
            previous_brush
        ))
        {
            ::Polygon(
                dc,
                native_points.data(),
                static_cast<int>(native_points.size())
            );

            restore_pen_and_brush(
                dc,
                pen,
                brush,
                previous_pen,
                previous_brush
            );
        }

        ReleaseDC(get_native_window(window_), dc);
    }

    void WindowsRenderer::present()
    {
        // GDI currently draws directly to the window.
        // Buffered presentation will be implemented in Stage 9.
    }
}