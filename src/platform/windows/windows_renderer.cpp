#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <vector>

#include <algorithm>

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
            HDC dc,
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

            previous_pen = SelectObject(dc, pen);
            previous_brush = SelectObject(dc, brush);

            if (!previous_pen || previous_pen == HGDI_ERROR ||
                !previous_brush || previous_brush == HGDI_ERROR)
            {
                if (previous_pen && previous_pen != HGDI_ERROR)
                    SelectObject(dc, previous_pen);

                if (previous_brush && previous_brush != HGDI_ERROR)
                    SelectObject(dc, previous_brush);

                DeleteObject(brush);
                DeleteObject(pen);

                return false;
            }

            return true;
        }

        void restore_pen_and_brush(
            HDC dc,
            HPEN pen,
            HBRUSH brush,
            HGDIOBJ previous_pen,
            HGDIOBJ previous_brush
        )
        {
            SelectObject(dc, previous_brush);
            SelectObject(dc, previous_pen);

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
        if (initialized_)
            return true;

        if (!window_.is_valid())
            return false;

        HWND window = get_native_window(window_);

        if (!IsWindow(window))
            return false;

        RECT bounds{};

        if (!GetClientRect(window, &bounds))
            return false;

        HDC window_dc = GetDC(window);

        if (!window_dc)
            return false;

        buffer_dc_ = CreateCompatibleDC(window_dc);

        ReleaseDC(window, window_dc);

        if (!buffer_dc_)
            return false;

        initialized_ = true;

        const int width = bounds.right - bounds.left;
        const int height = bounds.bottom - bounds.top;

        if (!resize_buffer(
            width > 0 ? width : 1,
            height > 0 ? height : 1
        ))
        {
            shutdown();
            return false;
        }

        return true;
    }

    void WindowsRenderer::shutdown() noexcept
    {
        release_buffer();
        initialized_ = false;
    }

    bool WindowsRenderer::is_initialized() const noexcept
    {
        return initialized_;
    }

    bool WindowsRenderer::resize() noexcept
    {
        if (!initialized_)
            return false;

        HWND window = get_native_window(window_);

        if (!IsWindow(window))
            return false;

        RECT bounds{};

        if (!GetClientRect(window, &bounds))
            return false;

        const int width = bounds.right - bounds.left;
        const int height = bounds.bottom - bounds.top;

        if (width <= 0 || height <= 0)
            return true;

        return resize_buffer(width, height);
    }

    void WindowsRenderer::paint(HDC target) noexcept
    {
        if (!initialized_ || !buffer_dc_ || !target)
            return;

        HWND window = get_native_window(window_);

        if (!IsWindow(window))
            return;

        RECT bounds{};

        if (!GetClientRect(window, &bounds))
            return;

        const int width = bounds.right - bounds.left;
        const int height = bounds.bottom - bounds.top;

        if (width <= 0 || height <= 0)
            return;

        if (!resize_buffer(width, height))
            return;

        BitBlt(
            target,
            0,
            0,
            width,
            height,
            buffer_dc_,
            0,
            0,
            SRCCOPY
        );
    }

    bool WindowsRenderer::resize_buffer(int width, int height) noexcept
    {
        if (!initialized_ || !buffer_dc_)
            return false;

        if (width <= 0 || height <= 0)
            return false;

        if (width == buffer_width_ && height == buffer_height_)
            return true;

        HWND window = get_native_window(window_);

        if (!IsWindow(window))
            return false;

        HDC window_dc = GetDC(window);

        if (!window_dc)
            return false;

        HBITMAP new_bitmap = CreateCompatibleBitmap(
            window_dc,
            width,
            height
        );

        ReleaseDC(window, window_dc);

        if (!new_bitmap)
            return false;

        HGDIOBJ old_bitmap = SelectObject(buffer_dc_, new_bitmap);

        if (!old_bitmap || old_bitmap == HGDI_ERROR)
        {
            DeleteObject(new_bitmap);
            return false;
        }

        if (!previous_bitmap_)
            previous_bitmap_ = old_bitmap;

        if (buffer_bitmap_)
        {
            HDC old_dc = CreateCompatibleDC(buffer_dc_);

            if (old_dc)
            {
                HGDIOBJ previous = SelectObject(old_dc, old_bitmap);

                if (previous && previous != HGDI_ERROR)
                {
                    BitBlt(
                        buffer_dc_,
                        0,
                        0,
                        (std::min)(width, buffer_width_),
                        (std::min)(height, buffer_height_),
                        old_dc,
                        0,
                        0,
                        SRCCOPY
                    );

                    SelectObject(old_dc, previous);
                }

                DeleteDC(old_dc);
            }

            DeleteObject(buffer_bitmap_);
        }

        buffer_bitmap_ = new_bitmap;
        buffer_width_ = width;
        buffer_height_ = height;

        return true;
    }

    void WindowsRenderer::release_buffer() noexcept
    {
        if (buffer_dc_)
        {
            if (previous_bitmap_)
            {
                SelectObject(buffer_dc_, previous_bitmap_);
                previous_bitmap_ = nullptr;
            }

            if (buffer_bitmap_)
            {
                DeleteObject(buffer_bitmap_);
                buffer_bitmap_ = nullptr;
            }

            DeleteDC(buffer_dc_);
            buffer_dc_ = nullptr;
        }

        buffer_width_ = 0;
        buffer_height_ = 0;
    }

    void WindowsRenderer::clear(const Color& color)
    {
        if (!initialized_ || !buffer_dc_)
            return;

        RECT bounds
        {
            0,
            0,
            buffer_width_,
            buffer_height_
        };

        HBRUSH brush = CreateSolidBrush(to_native_color(color));

        if (brush)
        {
            FillRect(buffer_dc_, &bounds, brush);
            DeleteObject(brush);
        }
    }

    void WindowsRenderer::draw_pixel(
        const Point& position,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_)
            return;

        SetPixelV(
            buffer_dc_,
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            to_native_color(color)
        );
    }

    void WindowsRenderer::draw_line(
        const Point& start,
        const Point& end,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_)
            return;

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (!pen)
            return;

        HGDIOBJ previous = SelectObject(buffer_dc_, pen);

        if (!previous || previous == HGDI_ERROR)
        {
            DeleteObject(pen);
            return;
        }

        MoveToEx(
            buffer_dc_,
            static_cast<int>(start.x),
            static_cast<int>(start.y),
            nullptr
        );

        LineTo(
            buffer_dc_,
            static_cast<int>(end.x),
            static_cast<int>(end.y)
        );

        SelectObject(buffer_dc_, previous);
        DeleteObject(pen);
    }

    void WindowsRenderer::draw_rectangle(
        const Rectangle& rectangle,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_)
            return;

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (!pen)
            return;

        HGDIOBJ previous_pen = SelectObject(buffer_dc_, pen);
        HGDIOBJ previous_brush = SelectObject(
            buffer_dc_,
            GetStockObject(NULL_BRUSH)
        );

        if (!previous_pen || previous_pen == HGDI_ERROR ||
            !previous_brush || previous_brush == HGDI_ERROR)
        {
            if (previous_pen && previous_pen != HGDI_ERROR)
                SelectObject(buffer_dc_, previous_pen);

            if (previous_brush && previous_brush != HGDI_ERROR)
                SelectObject(buffer_dc_, previous_brush);

            DeleteObject(pen);
            return;
        }

        ::Rectangle(
            buffer_dc_,
            static_cast<int>(rectangle.left()),
            static_cast<int>(rectangle.top()),
            static_cast<int>(rectangle.right()),
            static_cast<int>(rectangle.bottom())
        );

        SelectObject(buffer_dc_, previous_brush);
        SelectObject(buffer_dc_, previous_pen);

        DeleteObject(pen);
    }

    void WindowsRenderer::fill_rectangle(
        const Rectangle& rectangle,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_)
            return;

        RECT bounds = to_native_rectangle(rectangle);

        HBRUSH brush = CreateSolidBrush(to_native_color(color));

        if (brush)
        {
            FillRect(buffer_dc_, &bounds, brush);
            DeleteObject(brush);
        }
    }

    void WindowsRenderer::draw_ellipse(
        const Rectangle& bounds,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_)
            return;

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (!pen)
            return;

        HGDIOBJ previous_pen = SelectObject(buffer_dc_, pen);
        HGDIOBJ previous_brush = SelectObject(
            buffer_dc_,
            GetStockObject(NULL_BRUSH)
        );

        if (!previous_pen || previous_pen == HGDI_ERROR ||
            !previous_brush || previous_brush == HGDI_ERROR)
        {
            if (previous_pen && previous_pen != HGDI_ERROR)
                SelectObject(buffer_dc_, previous_pen);

            if (previous_brush && previous_brush != HGDI_ERROR)
                SelectObject(buffer_dc_, previous_brush);

            DeleteObject(pen);
            return;
        }

        RECT native_bounds = to_native_rectangle(bounds);

        ::Ellipse(
            buffer_dc_,
            native_bounds.left,
            native_bounds.top,
            native_bounds.right,
            native_bounds.bottom
        );

        SelectObject(buffer_dc_, previous_brush);
        SelectObject(buffer_dc_, previous_pen);

        DeleteObject(pen);
    }

    void WindowsRenderer::fill_ellipse(
        const Rectangle& bounds,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_)
            return;

        HPEN pen;
        HBRUSH brush;
        HGDIOBJ previous_pen;
        HGDIOBJ previous_brush;

        if (!create_pen_and_brush(
            buffer_dc_,
            color,
            pen,
            brush,
            previous_pen,
            previous_brush
        ))
            return;

        RECT native_bounds = to_native_rectangle(bounds);

        ::Ellipse(
            buffer_dc_,
            native_bounds.left,
            native_bounds.top,
            native_bounds.right,
            native_bounds.bottom
        );

        restore_pen_and_brush(
            buffer_dc_,
            pen,
            brush,
            previous_pen,
            previous_brush
        );
    }

    void WindowsRenderer::draw_circle(
        const Point& center,
        Float32 radius,
        const Color& color
    )
    {
        if (radius < 0.0f)
            return;

        Rectangle bounds(
            Point(center.x - radius, center.y - radius),
            Size(radius * 2.0f, radius * 2.0f)
        );

        draw_ellipse(bounds, color);
    }

    void WindowsRenderer::fill_circle(
        const Point& center,
        Float32 radius,
        const Color& color
    )
    {
        if (radius < 0.0f)
            return;

        Rectangle bounds(
            Point(center.x - radius, center.y - radius),
            Size(radius * 2.0f, radius * 2.0f)
        );

        fill_ellipse(bounds, color);
    }

    void WindowsRenderer::draw_polygon(
        std::span<const Point> points,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_ || points.size() < 3)
            return;

        std::vector<POINT> native_points;
        native_points.reserve(points.size());

        for (const Point& point : points)
            native_points.push_back(to_native_point(point));

        HPEN pen = CreatePen(PS_SOLID, 1, to_native_color(color));

        if (!pen)
            return;

        HGDIOBJ previous_pen = SelectObject(buffer_dc_, pen);
        HGDIOBJ previous_brush = SelectObject(
            buffer_dc_,
            GetStockObject(NULL_BRUSH)
        );

        if (!previous_pen || previous_pen == HGDI_ERROR ||
            !previous_brush || previous_brush == HGDI_ERROR)
        {
            if (previous_pen && previous_pen != HGDI_ERROR)
                SelectObject(buffer_dc_, previous_pen);

            if (previous_brush && previous_brush != HGDI_ERROR)
                SelectObject(buffer_dc_, previous_brush);

            DeleteObject(pen);
            return;
        }

        ::Polygon(
            buffer_dc_,
            native_points.data(),
            static_cast<int>(native_points.size())
        );

        SelectObject(buffer_dc_, previous_brush);
        SelectObject(buffer_dc_, previous_pen);

        DeleteObject(pen);
    }

    void WindowsRenderer::fill_polygon(
        std::span<const Point> points,
        const Color& color
    )
    {
        if (!initialized_ || !buffer_dc_ || points.size() < 3)
            return;

        std::vector<POINT> native_points;
        native_points.reserve(points.size());

        for (const Point& point : points)
            native_points.push_back(to_native_point(point));

        HPEN pen;
        HBRUSH brush;
        HGDIOBJ previous_pen;
        HGDIOBJ previous_brush;

        if (!create_pen_and_brush(
            buffer_dc_,
            color,
            pen,
            brush,
            previous_pen,
            previous_brush
        ))
            return;

        ::Polygon(
            buffer_dc_,
            native_points.data(),
            static_cast<int>(native_points.size())
        );

        restore_pen_and_brush(
            buffer_dc_,
            pen,
            brush,
            previous_pen,
            previous_brush
        );
    }

    void WindowsRenderer::present()
    {
        if (!initialized_)
            return;

        HWND window = get_native_window(window_);

        if (!IsWindow(window))
            return;

        InvalidateRect(window, nullptr, FALSE);
    }
}