#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstdint>
#include <utility>

#include <eaglegraphics/integration/graphics_context.h>
#include <eaglegraphics/core/rectangle.h>

using eaglegraphics::GraphicsContext;
using eaglegraphics::WindowHandle;
using eaglegraphics::Color;

namespace
{
    constexpr wchar_t WINDOW_CLASS_NAME[] =
        L"EagleGraphicsIntegrationTest";

    GraphicsContext* active_context = nullptr;

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
            if (active_context && active_context->is_initialized())
            {
                active_context->paint();
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
        }

        return DefWindowProcW(window, message, wparam, lparam);
    }
}

int main()
{
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSW window_class{};

    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance;
    window_class.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassW(&window_class))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return 1;
    }

    HWND window = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"EagleGraphics Integration Test",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        320,
        240,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!window)
        return 2;

    GraphicsContext invalid_context(WindowHandle{});

    if (invalid_context.initialize())
        return 3;

    if (invalid_context.is_initialized())
        return 4;

    if (invalid_context.renderer() != nullptr)
        return 5;

    if (invalid_context.resize())
        return 6;

    invalid_context.present();
    invalid_context.paint();
    invalid_context.shutdown();

    GraphicsContext context(
        WindowHandle(reinterpret_cast<std::uintptr_t>(window))
    );

    active_context = &context;

    if (!context.initialize())
        return 7;

    if (!context.is_initialized())
        return 8;

    if (context.renderer() == nullptr)
        return 9;

    if (!context.resize())
        return 10;

    auto* renderer = context.renderer();

    renderer->clear(Color(18, 22, 35));

    renderer->fill_rectangle(
        eaglegraphics::Rectangle(20.0f, 20.0f, 100.0f, 80.0f),
        Color(139, 92, 246)
    );

    renderer->draw_rectangle(
        eaglegraphics::Rectangle(140.0f, 20.0f, 100.0f, 80.0f),
        Color(0, 200, 255)
    );

    context.present();

    InvalidateRect(window, nullptr, FALSE);
    UpdateWindow(window);

    context.shutdown();

    if (context.is_initialized())
        return 11;

    if (context.renderer() != nullptr)
        return 12;

    if (context.resize())
        return 13;

    if (!context.initialize())
        return 14;

    if (!context.is_initialized())
        return 15;

    GraphicsContext moved_context(std::move(context));

    if (!moved_context.is_initialized())
        return 16;

    if (context.is_initialized())
        return 17;

    if (context.renderer() != nullptr)
        return 18;

    GraphicsContext assigned_context(WindowHandle{});

    assigned_context = std::move(moved_context);

    if (!assigned_context.is_initialized())
        return 19;

    if (moved_context.is_initialized())
        return 20;

    active_context = &assigned_context;

    assigned_context.shutdown();

    if (assigned_context.is_initialized())
        return 21;

    if (!assigned_context.initialize())
        return 22;

    assigned_context.shutdown();

    active_context = nullptr;

    DestroyWindow(window);

    return 0;
}