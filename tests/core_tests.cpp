#include <cassert>

#include <eaglegraphics/core/graphics.h>
#include <eaglegraphics/core/color.h>

using namespace eaglegraphics;

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

    return 0;
}