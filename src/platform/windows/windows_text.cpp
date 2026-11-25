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
        if (!initialized_ || !buffer_dc_ || text.empty())
            return;

        if (font.size <= 0.0f || font.family.empty())
            return;

        if (text.size() > static_cast<std::size_t>(INT_MAX))
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
            return;

        std::wstring wide_text(
            static_cast<std::size_t>(required_size),
            L'\0'
        );

        if (MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            text.data(),
            static_cast<int>(text.size()),
            wide_text.data(),
            required_size
        ) <= 0)
            return;

        int family_size = MultiByteToWideChar(
            CP_UTF8,
            0,
            font.family.c_str(),
            -1,
            nullptr,
            0
        );

        if (family_size <= 0)
            return;

        std::wstring wide_family(
            static_cast<std::size_t>(family_size),
            L'\0'
        );

        if (MultiByteToWideChar(
            CP_UTF8,
            0,
            font.family.c_str(),
            -1,
            wide_family.data(),
            family_size
        ) <= 0)
            return;

        if (!wide_family.empty() && wide_family.back() == L'\0')
            wide_family.pop_back();

        LOGFONTW settings{};

        settings.lfHeight = -std::max(
            1,
            static_cast<int>(font.size)
        );

        settings.lfWeight = font.bold ? FW_BOLD : FW_NORMAL;
        settings.lfItalic = font.italic ? TRUE : FALSE;
        settings.lfCharSet = DEFAULT_CHARSET;
        settings.lfQuality = CLEARTYPE_QUALITY;

        lstrcpynW(
            settings.lfFaceName,
            wide_family.c_str(),
            LF_FACESIZE
        );

        HFONT native_font = CreateFontIndirectW(&settings);

        if (!native_font)
            return;

        HGDIOBJ previous_font = SelectObject(
            buffer_dc_,
            native_font
        );

        int previous_background_mode = SetBkMode(
            buffer_dc_,
            TRANSPARENT
        );

        COLORREF previous_text_color = SetTextColor(
            buffer_dc_,
            RGB(color.r, color.g, color.b)
        );

        TextOutW(
            buffer_dc_,
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            wide_text.c_str(),
            static_cast<int>(wide_text.size())
        );

        SetTextColor(buffer_dc_, previous_text_color);
        SetBkMode(buffer_dc_, previous_background_mode);

        SelectObject(buffer_dc_, previous_font);

        DeleteObject(native_font);
    }
}