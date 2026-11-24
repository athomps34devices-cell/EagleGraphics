cmake_minimum_required(VERSION 3.20)

project(EagleGraphics
    VERSION 0.1.0
    DESCRIPTION "A shared graphics framework for the Eagle software family"
    LANGUAGES CXX
)

add_library(eaglegraphics STATIC
    src/version.cpp
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