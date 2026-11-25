#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstdint>
#include <memory>

#include <eaglegraphics/platform/windows/windows_renderer.h>

using eaglegraphics::WindowsRenderer;
using eaglegraphics::WindowHandle;
using eaglegraphics::Color;
using eaglegraphics::Point;
using eaglegraphics::Size;

namespace
{
    std::unique_ptr<WindowsRenderer> renderer;

    constexpr wchar_t WINDOW_CLASS_NAME[] = L"EagleGraphicsDemo";

    LRESULT CALLBACK window_procedure(
        HWND window,
        UINT message,
        WPARAM wparam,
        LPARAM lparam
    )
    {
        switch (message)
        {
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            BeginPaint(window, &paint);
            EndPaint(window, &paint);

            if (renderer && renderer->is_initialized())
            {
                renderer->clear(Color(18, 22, 35));

                renderer->fill_rectangle(
                    eaglegraphics::Rectangle(Point(30, 30), Size(130, 80)),
                    Color(139, 92, 246)
                );

                renderer->draw_rectangle(
                    eaglegraphics::Rectangle(Point(190, 30), Size(130, 80)),
                    Color(0, 200, 255)
                );

                renderer->draw_line(
                    Point(30, 140),
                    Point(450, 140),
                    Color(255, 255, 255)
                );

                renderer->draw_pixel(
                    Point(240, 170),
                    Color(255, 80, 80)
                );

                renderer->fill_ellipse(
                    eaglegraphics::Rectangle(Point(30, 180), Size(100, 65)),
                    Color(40, 200, 120)
                );

                renderer->draw_ellipse(
                    eaglegraphics::Rectangle(Point(160, 180), Size(100, 65)),
                    Color(255, 200, 50)
                );

                renderer->fill_circle(
                    Point(340, 215),
                    35.0f,
                    Color(255, 80, 120)
                );

                renderer->draw_circle(
                    Point(440, 215),
                    35.0f,
                    Color(0, 200, 255)
                );

                const Point triangle[] =
                {
                    Point(30, 290),
                    Point(100, 240),
                    Point(170, 290)
                };

                renderer->fill_polygon(
                    triangle,
                    Color(139, 92, 246)
                );

                const Point polygon[] =
                {
                    Point(230, 245),
                    Point(290, 245),
                    Point(320, 285),
                    Point(260, 315),
                    Point(210, 285)
                };

                renderer->draw_polygon(
                    polygon,
                    Color(255, 255, 255)
                );

                renderer->present();

                renderer->draw_text(
                    Point(30, 330),
                    "Hello, EagleGraphics!",
                    eaglegraphics::Font("Arial", 22.0f, true, false),
                    Color(255, 255, 255)
                );

                renderer->draw_text(
                    Point(30, 360),
                    "Different fonts, sizes, and styles.",
                    eaglegraphics::Font("Consolas", 16.0f, false, true),
                    Color(0, 200, 255)
                );
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

    renderer = std::make_unique<WindowsRenderer>(
        WindowHandle(reinterpret_cast<std::uintptr_t>(window))
    );

    if (!renderer->initialize())
    {
        DestroyWindow(window);
        return 3;
    }

    ShowWindow(window, show_command);
    UpdateWindow(window);

    MSG message{};

    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    renderer->shutdown();
    renderer.reset();

    return static_cast<int>(message.wParam);
}