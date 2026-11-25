# EagleGraphics

EagleGraphics is a portable graphics engine designed to allow custom software to run without rewriting graphics systems for each project.

## About

EagleGraphics is a C++ graphics framework that is designed to be used with the Eagle software family to add graphics rendering to the apps.

This is also designed for independent programmers to use so they don't have to rewrite graphics systems from scratch.

## Features

This release includes:

- **Graphics Core** – Fundamental graphics data types and structures.
- **Color Management** – RGB and RGBA color support.
- **Rendering Interface** – A shared interface for graphics operations.
- **Platform Abstraction** – Separation between common graphics functionality and platform-specific implementations.
- **Windows Rendering Backend** – Native Windows rendering using GDI.
- **Drawing Primitives** – Pixels, lines, rectangles, ellipses, circles, and polygons.
- **Text Rendering** – UTF-8 text rendering with configurable font families, sizes, bold, and italic styles.
- **Double Buffering** – Off-screen rendering to reduce flickering.
- **Automatic Buffer Resizing** – Rendering buffers automatically adapt to window dimensions.

## Compatibility

This system currently only has a Windows backend. More backends will be added in future releases.

## Installation Guide

EagleGraphics requires **CMake 3.20 or newer** to run correctly.

If you don't have CMake, you can get it from their website:

**CMake:** [https://cmake.org/download/](https://cmake.org/download/)

1. Visit the official [CMake download page](https://cmake.org/download/).
2. Download the **Windows x64 Installer (.msi)**.
3. Run the installer.
4. Enable **Add CMake to the system PATH** when prompted.
5. Complete the installation.

After installation, open PowerShell and run:

```powershell
cmake --version
```

If CMake is installed correctly, it will display the version you installed.

## Installing the EagleGraphics SDK

Download `EagleGraphics-v1.0-SDK.zip` from the GitHub Releases.

Extract the ZIP file to where your project is.

Your extracted SDK should contain a structure similar to:

```text
EagleGraphics-SDK/
├── include/
│   └── eaglegraphics/
├── lib/
│   ├── libeaglegraphics.a
│   └── cmake/
│       └── EagleGraphics/
│           ├── EagleGraphicsConfig.cmake
│           ├── EagleGraphicsConfigVersion.cmake
│           └── EagleGraphicsTargets.cmake
```

### Configure your application

Open your application's `CMakeLists.txt`.

Add this code:

```cmake
find_package(EagleGraphics 1.0 REQUIRED CONFIG)

target_link_libraries(YourApplication
    PRIVATE
        EagleGraphics::EagleGraphics
)
```

Replace `YourApplication` with the name of your program.

Configure your application using:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Libraries/EagleGraphics-SDK"
```

Replace the path with the location where the SDK is extracted.

You can now include EagleGraphics headers in your C++ source files:

```cpp
#include <eaglegraphics/core/color.h>
#include <eaglegraphics/core/point.h>
#include <eaglegraphics/core/font.h>
#include <eaglegraphics/rendering/renderer.h>
```

### Building your application

When you finish coding, build your application with this command:

```powershell
cmake --build build
```

### Important compatibility information

- EagleGraphics v1.0 currently provides a Windows rendering backend.
- The SDK library must be compatible with your application's compiler and architecture.
- The Windows SDK build is intended for compatible Windows C++ applications.
- Linux and macOS support will be introduced in future releases.

### Troubleshooting

**CMake cannot find EagleGraphics**

Check that `CMAKE_PREFIX_PATH` points to where the extracted SDK is, not its `include` or `lib` subdirectory.

**The compiler cannot find an EagleGraphics header**

Make sure you linked your application against `EagleGraphics::EagleGraphics`.

**Linker errors occur**

Check that your application and EagleGraphics SDK use compatible compilers, architectures, and build configurations.

## License

EagleGraphics is distributed under the license included in this repository.

See [LICENSE](LICENSE) for details.

## Credits

**Project:** EagleGraphics  
**Developer:** The Nerds Society

---

Copyright © 2026 The Nerds Society.