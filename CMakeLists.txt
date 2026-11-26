cmake_minimum_required(VERSION 3.20)

project(EagleGraphics
    VERSION 1.0.0
    DESCRIPTION "A shared graphics framework for the Eagle software family"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(eaglegraphics STATIC
    src/version.cpp
    src/core/graphics.cpp
    src/integration/graphics_context.cpp
)

add_library(EagleGraphics::EagleGraphics ALIAS eaglegraphics)

set_target_properties(eaglegraphics PROPERTIES
    EXPORT_NAME EagleGraphics
    CXX_EXTENSIONS OFF
)

target_compile_features(eaglegraphics PUBLIC cxx_std_20)

target_include_directories(eaglegraphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(MSVC)
    target_compile_options(eaglegraphics PRIVATE
        /W4
        /permissive-
    )
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

install(TARGETS eaglegraphics
    EXPORT EagleGraphicsTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT EagleGraphicsTargets
    FILE EagleGraphicsTargets.cmake
    NAMESPACE EagleGraphics::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EagleGraphics
)

configure_package_config_file(
    cmake/EagleGraphicsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EagleGraphicsConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EagleGraphics
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/EagleGraphicsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EagleGraphicsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EagleGraphicsConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EagleGraphics
)

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

    add_executable(eaglegraphics_windows_integration_tests
        tests/windows_integration_tests.cpp
    )

    target_link_libraries(eaglegraphics_windows_integration_tests
        PRIVATE
            EagleGraphics::EagleGraphics
            user32
            gdi32
    )

    add_test(
        NAME EagleGraphicsWindowsIntegrationTests
        COMMAND eaglegraphics_windows_integration_tests
    )
endif()
