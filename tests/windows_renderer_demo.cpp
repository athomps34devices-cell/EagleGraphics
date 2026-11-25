#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

#include <eaglegraphics/integration/graphics_context.h>

using eaglegraphics::GraphicsContext;
using eaglegraphics::WindowHandle;
using eaglegraphics::Color;
using eaglegraphics::Point;
using eaglegraphics::Size;
using GraphicsRectangle = eaglegraphics::Rectangle;
using eaglegraphics::Float32;

namespace
{
    std::unique_ptr<GraphicsContext> graphics;

    constexpr wchar_t WINDOW_CLASS_NAME[] = L"EagleGraphicsDemo";

    constexpr float REFERENCE_WIDTH = 540.0f;
    constexpr float REFERENCE_HEIGHT = 460.0f;

    struct ScaleInfo
    {
        float scale;
        float offset_x;
        float offset_y;
    };

    ScaleInfo calculate_scale(int width, int height)
    {
        float scale_x = static_cast<float>(width) / REFERENCE_WIDTH;
        float scale_y = static_cast<float>(height) / REFERENCE_HEIGHT;

        float scale = (std::min)(scale_x, scale_y);

        return
        {
            scale,
            (static_cast<float>(width) - REFERENCE_WIDTH * scale) / 2.0f,
            (static_cast<float>(height) - REFERENCE_HEIGHT * scale) / 2.0f
        };
    }

    Point scale_point(const Point& point, const ScaleInfo& info)
    {
        return Point(
            static_cast<int>(point.x * info.scale + info.offset_x),
            static_cast<int>(point.y * info.scale + info.offset_y)
        );
    }

    Size scale_size(const Size& size, const ScaleInfo& info)
    {
        return Size(
            static_cast<int>(size.width * info.scale),
            static_cast<int>(size.height * info.scale)
        );
    }

    GraphicsRectangle scale_rectangle(
        const GraphicsRectangle& rectangle,
        const ScaleInfo& info
    )
    {
        return GraphicsRectangle(
            scale_point(rectangle.position, info),
            scale_size(rectangle.size, info)
        );
    }

    Float32 scale_radius(Float32 radius, const ScaleInfo& info)
    {
        return radius * info.scale;
    }

    eaglegraphics::Font scale_font(
        const eaglegraphics::Font& font,
        const ScaleInfo& info
    )
    {
        return eaglegraphics::Font(
            font.family,
            font.size * info.scale,
            font.bold,
            font.italic
        );
    }

    void render_scene(HWND window)
    {
        if (!graphics || !graphics->is_initialized())
            return;

        RECT client{};

        if (!GetClientRect(window, &client))
            return;

        int width = client.right - client.left;
        int height = client.bottom - client.top;

        if (width <= 0 || height <= 0)
            return;

        if (!graphics->resize())
            return;

        auto* renderer = graphics->renderer();

        if (!renderer)
            return;

        ScaleInfo info = calculate_scale(width, height);

        const std::wstring title =
            L"EagleGraphics - Scale: " +
            std::to_wstring(info.scale) +
            L" | Size: " +
            std::to_wstring(width) +
            L"x" +
            std::to_wstring(height);

        SetWindowTextW(window, title.c_str());

        renderer->clear(Color(18, 22, 35));

        renderer->fill_rectangle(
            scale_rectangle(
                GraphicsRectangle(Point(30, 30), Size(130, 80)),
                info
            ),
            Color(139, 92, 246)
        );

        renderer->draw_rectangle(
            scale_rectangle(
                GraphicsRectangle(Point(190, 30), Size(130, 80)),
                info
            ),
            Color(0, 200, 255)
        );

        renderer->draw_line(
            scale_point(Point(30, 140), info),
            scale_point(Point(450, 140), info),
            Color(255, 255, 255)
        );

        renderer->draw_pixel(
            scale_point(Point(240, 170), info),
            Color(255, 80, 80)
        );

        renderer->fill_ellipse(
            scale_rectangle(
                GraphicsRectangle(Point(30, 180), Size(100, 65)),
                info
            ),
            Color(40, 200, 120)
        );

        renderer->draw_ellipse(
            scale_rectangle(
                GraphicsRectangle(Point(160, 180), Size(100, 65)),
                info
            ),
            Color(255, 200, 50)
        );

        renderer->fill_circle(
            scale_point(Point(340, 215), info),
            scale_radius(35.0f, info),
            Color(255, 80, 120)
        );

        renderer->draw_circle(
            scale_point(Point(440, 215), info),
            scale_radius(35.0f, info),
            Color(0, 200, 255)
        );

        const Point triangle[] =
        {
            scale_point(Point(30, 290), info),
            scale_point(Point(100, 240), info),
            scale_point(Point(170, 290), info)
        };

        renderer->fill_polygon(
            triangle,
            Color(139, 92, 246)
        );

        const Point polygon[] =
        {
            scale_point(Point(230, 245), info),
            scale_point(Point(290, 245), info),
            scale_point(Point(320, 285), info),
            scale_point(Point(260, 315), info),
            scale_point(Point(210, 285), info)
        };

        renderer->draw_polygon(
            polygon,
            Color(255, 255, 255)
        );

        renderer->draw_text(
            scale_point(Point(30, 330), info),
            "Hello, EagleGraphics!",
            scale_font(
                eaglegraphics::Font("Arial", 22.0f, true, false),
                info
            ),
            Color(255, 255, 255)
        );

        renderer->draw_text(
            scale_point(Point(30, 360), info),
            "Different fonts, sizes, and styles.",
            scale_font(
                eaglegraphics::Font("Consolas", 16.0f, false, true),
                info
            ),
            Color(0, 200, 255)
        );

        graphics->present();
    }

    LRESULT CALLBACK window_procedure(
        HWND window,
        UINT message,
        WPARAM wparam,
        LPARAM lparam
    )
    {
        switch (message)
        {
        case WM_SIZE:
        {
            if (graphics && graphics->is_initialized() &&
                wparam != SIZE_MINIMIZED)
            {
                render_scene(window);
            }

            return 0;
        }

        case WM_PAINT:
        {
            if (graphics && graphics->is_initialized())
            {
                graphics->paint();
            }
            else
            {
                PAINTSTRUCT paint{};
                BeginPaint(window, &paint);
                EndPaint(window, &paint);
            }

            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(window, message, wparam, lparam);
    }
}

int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE,
    LPSTR,
    int show_command
)
{
    WNDCLASSW window_class{};

    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance;
    window_class.lpszClassName = WINDOW_CLASS_NAME;
    window_class.hCursor = LoadCursorW(
        nullptr,
        MAKEINTRESOURCEW(32512)
    );

    if (!RegisterClassW(&window_class))
        return 1;

    HWND window = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"EagleGraphics - Drawing Primitives Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        540,
        460,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!window)
        return 2;

    graphics = std::make_unique<GraphicsContext>(
        WindowHandle(reinterpret_cast<std::uintptr_t>(window))
    );

    if (!graphics->initialize())
    {
        graphics.reset();
        DestroyWindow(window);
        return 3;
    }

    render_scene(window);

    ShowWindow(window, show_command);
    UpdateWindow(window);

    MSG message{};

    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    graphics->shutdown();
    graphics.reset();

    return static_cast<int>(message.wParam);
}