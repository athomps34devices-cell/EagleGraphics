#include <cassert>

#include <eaglegraphics/core/graphics.h>
#include <eaglegraphics/core/color.h>
#include <eaglegraphics/rendering/renderer.h>
#include <eaglegraphics/platform/platform.h>
#include <eaglegraphics/platform/window_handle.h>

using namespace eaglegraphics;

class TestRenderer : public Renderer
{
public:
    int clear_calls = 0;
    int pixel_calls = 0;
    int line_calls = 0;
    int rectangle_calls = 0;
    int fill_calls = 0;
    int present_calls = 0;

    void clear(const Color&) override
    {
        clear_calls++;
    }

    void draw_pixel(const Point&, const Color&) override
    {
        pixel_calls++;
    }

    void draw_line(const Point&, const Point&, const Color&) override
    {
        line_calls++;
    }

    void draw_rectangle(const Rectangle&, const Color&) override
    {
        rectangle_calls++;
    }

    void fill_rectangle(const Rectangle&, const Color&) override
    {
        fill_calls++;
    }

    void present() override
    {
        present_calls++;
    }
};

int main()
{
    Point point(10.0f, 20.0f);

    assert(point.x == 10.0f);
    assert(point.y == 20.0f);

    Size size(100.0f, 50.0f);

    assert(size.width == 100.0f);
    assert(size.height == 50.0f);

    Rectangle rectangle(10.0f, 20.0f, 100.0f, 50.0f);

    assert(rectangle.left() == 10.0f);
    assert(rectangle.top() == 20.0f);
    assert(rectangle.right() == 110.0f);
    assert(rectangle.bottom() == 70.0f);

    assert(rectangle.contains(Point(50.0f, 40.0f)));
    assert(!rectangle.contains(Point(200.0f, 200.0f)));

    GraphicsInfo info = graphics_info();

    assert(info.api_major == 0);
    assert(info.api_minor == 1);
    assert(info.api_patch == 0);

    Color default_color;

    assert(default_color.r == 0);
    assert(default_color.g == 0);
    assert(default_color.b == 0);
    assert(default_color.a == 255);

    Color red(255, 0, 0);

    assert(red.r == 255);
    assert(red.g == 0);
    assert(red.b == 0);
    assert(red.a == 255);
    assert(red.is_opaque());
    assert(!red.is_transparent());

    Color transparent_blue(0, 0, 255, 0);

    assert(transparent_blue.is_transparent());
    assert(!transparent_blue.is_opaque());

    Color translucent_green(0, 255, 0, 128);

    assert(translucent_green.a == 128);
    assert(!translucent_green.is_opaque());
    assert(!translucent_green.is_transparent());

    Color modified_color = red.with_alpha(128);

    assert(modified_color.r == 255);
    assert(modified_color.g == 0);
    assert(modified_color.b == 0);
    assert(modified_color.a == 128);

    assert(red.a == 255);

    assert(red == Color(255, 0, 0));
    assert(red != transparent_blue);

    TestRenderer renderer;

    renderer.clear(Color(0, 0, 0));
    renderer.draw_pixel(Point(10, 10), Color(255, 0, 0));
    renderer.draw_line(Point(0, 0), Point(100, 100), Color(0, 255, 0));
    renderer.draw_rectangle(Rectangle(10, 10, 50, 50), Color(0, 0, 255));
    renderer.fill_rectangle(Rectangle(20, 20, 30, 30), Color(255, 255, 0));
    renderer.present();

    assert(renderer.clear_calls == 1);
    assert(renderer.pixel_calls == 1);
    assert(renderer.line_calls == 1);
    assert(renderer.rectangle_calls == 1);
    assert(renderer.fill_calls == 1);
    assert(renderer.present_calls == 1);

    constexpr Platform platform = current_platform();

    assert(platform != Platform::Unknown);

    constexpr WindowHandle invalid_handle;
    constexpr WindowHandle valid_handle(12345);

    assert(!invalid_handle.is_valid());
    assert(valid_handle.is_valid());

    assert(static_cast<bool>(valid_handle));
    assert(!static_cast<bool>(invalid_handle));

    assert(valid_handle.value == 12345);
    assert(valid_handle == WindowHandle(12345));
    assert(valid_handle != invalid_handle);

    return 0;
}