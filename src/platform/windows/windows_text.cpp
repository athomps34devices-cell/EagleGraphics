#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <string>
#include <algorithm>

#include <eaglegraphics/platform/windows/windows_renderer.h>

namespace eaglegraphics
{
    void WindowsRenderer::draw_text(
        const Point& position,
        std::string_view text,
        const Font& font,
        const Color& color
    )
    {
        if (!initialized_ || text.empty())
            return;

        if (font.size <= 0.0f || font.family.empty())
            return;

        HWND window = reinterpret_cast<HWND>(window_.value);

        HDC dc = GetDC(window);

        if (!dc)
            return;

        int required_size = MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            nullptr,
            0
        );

        if (required_size <= 0)
        {
            ReleaseDC(window, dc);
            return;
        }

        std::wstring wide_text(
            static_cast<std::size_t>(required_size),
            L'\0'
        );

        MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            wide_text.data(),
            required_size
        );

        int family_size = MultiByteToWideChar(
            CP_UTF8,
            0,
            font.family.c_str(),
            -1,
            nullptr,
            0
        );

        if (family_size <= 0)
        {
            ReleaseDC(window, dc);
            return;
        }

        std::wstring wide_family(
            static_cast<std::size_t>(family_size),
            L'\0'
        );

        MultiByteToWideChar(
            CP_UTF8,
            0,
            font.family.c_str(),
            -1,
            wide_family.data(),
            family_size
        );

        if (!wide_family.empty() && wide_family.back() == L'\0')
            wide_family.pop_back();

        LOGFONTW font_settings{};

        font_settings.lfHeight = -std::max(
            1,
            static_cast<int>(font.size)
        );

        font_settings.lfWeight = font.bold
            ? FW_BOLD
            : FW_NORMAL;

        font_settings.lfItalic = font.italic ? TRUE : FALSE;

        font_settings.lfCharSet = DEFAULT_CHARSET;
        font_settings.lfQuality = CLEARTYPE_QUALITY;

        lstrcpynW(
            font_settings.lfFaceName,
            wide_family.c_str(),
            LF_FACESIZE
        );

        HFONT native_font = CreateFontIndirectW(&font_settings);

        if (!native_font)
        {
            ReleaseDC(window, dc);
            return;
        }

        HGDIOBJ previous_font = SelectObject(dc, native_font);

        int previous_background_mode = SetBkMode(dc, TRANSPARENT);

        COLORREF previous_text_color = SetTextColor(
            dc,
            RGB(color.r, color.g, color.b)
        );

        TextOutW(
            dc,
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            wide_text.c_str(),
            static_cast<int>(wide_text.size())
        );

        SetTextColor(dc, previous_text_color);
        SetBkMode(dc, previous_background_mode);

        SelectObject(dc, previous_font);

        DeleteObject(native_font);

        ReleaseDC(window, dc);
    }
}