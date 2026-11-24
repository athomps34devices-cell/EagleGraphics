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
                    eaglegraphics::Rectangle(Point(40, 40), Size(180, 100)),
                    Color(139, 92, 246)
                );

                renderer->draw_rectangle(
                    eaglegraphics::Rectangle(Point(260, 40), Size(180, 100)),
                    Color(0, 200, 255)
                );

                renderer->draw_line(
                    Point(40, 200),
                    Point(440, 200),
                    Color(255, 255, 255)
                );

                renderer->draw_pixel(
                    Point(240, 260),
                    Color(255, 80, 80)
                );

                renderer->fill_rectangle(
                    eaglegraphics::Rectangle(Point(40, 240), Size(100, 70)),
                    Color(40, 200, 120)
                );

                renderer->present();
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
    {
        return 1;
    }

    HWND window = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"EagleGraphics - Windows Rendering Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        540,
        380,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!window)
    {
        return 2;
    }

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