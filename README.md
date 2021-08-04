# binviz
Binary visualization tool.

Allows you to load a binary and pan/zoom around its content. Each byte (or 4 bytes in 4-byte mode) is represented by a single pixel/color.

## Features
The basic modes available are:

* **Threshold**: Visualize floating point values of a given range, with specific colors
* **Palette**: View each byte value as a specific color
* **RGBA**: View bytes interpreted as RGBA colors

## Screenshots
##### Note: These might be outdated
![](screenshots/1.png?raw=true "")
![](screenshots/2.png?raw=true "")
![](screenshots/3.png?raw=true "")

## Build instructions
- Get [vcpkg](https://github.com/microsoft/vcpkg).
- Make an environment variable called `${VCPKG_ROOT}` point to its installation directory.
- Make sure `vcpkg` is accessible from the command line. On Windows you need to add the dir to the `${PATH}` environment variable.
- Open the CMakeLists.txt in Visual Studio or CLion, or use CMake to otherwise create build files / build.

## Motivation
The initial motivation for creating this was exploring PS2 memory dumps and game files, specifically for finding clustered floating point numbers in game memory that could be part of interesting structures, like meshes. PS2 also uses paletted textures a lot, so the "Palette" feature was developed shortly after.

Despite the original use case, it can of course be used to visualize any file.

The floating point view in particular is very useful for game reverse engineering. To my knowledge there are not other tools that do this.

Cheat Engine has a "Graphical memory viewer" (no float support), but It's very limited feature-wise and _really_ slow compared to binviz (however, it works with RAM).
