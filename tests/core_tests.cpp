#include <cassert>

#include <eaglegraphics/core/graphics.h>

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

    return 0;
}