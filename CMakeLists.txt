cmake_minimum_required(VERSION 3.20)

project(EagleGraphics
    VERSION 0.1.0
    DESCRIPTION "A shared graphics framework for the Eagle software family"
    LANGUAGES CXX
)

add_library(eaglegraphics STATIC
    src/version.cpp
    src/core/graphics.cpp
)

add_library(EagleGraphics::EagleGraphics ALIAS eaglegraphics)

target_compile_features(eaglegraphics PUBLIC cxx_std_20)

target_include_directories(eaglegraphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(eaglegraphics PROPERTIES
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(eaglegraphics PRIVATE /W4 /permissive-)
else()
    target_compile_options(eaglegraphics PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

if(WIN32)
    target_sources(eaglegraphics PRIVATE
        src/platform/windows/windows_renderer.cpp
        src/platform/windows/windows_text.cpp
    )

    target_link_libraries(eaglegraphics PRIVATE
        user32
        gdi32
    )
endif()

include(CTest)

if(BUILD_TESTING)
    add_executable(eaglegraphics_tests
        tests/core_tests.cpp
    )

    target_link_libraries(eaglegraphics_tests
        PRIVATE EagleGraphics::EagleGraphics
    )

    add_test(
        NAME EagleGraphicsCoreTests
        COMMAND eaglegraphics_tests
    )
endif()

if(WIN32 AND BUILD_TESTING)
    add_executable(eaglegraphics_windows_demo WIN32
        tests/windows_renderer_demo.cpp
    )

    target_link_libraries(eaglegraphics_windows_demo
        PRIVATE
            EagleGraphics::EagleGraphics
            user32
            gdi32
    )
endif()