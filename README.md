# binviz
Binary visualization tool.
Allows you to load a binary and pan/zoom around its vizualized content.

# Features
The basic modes available are:
**Threshold**: Visualize floating point values in specific ranges
**Palette**: View each byte value as a specific color
**RGBA**: View bytes as RGBA colors (currently available only as a compile-time flag)

# Screenshots
![](screenshots/1.png?raw=true "")
![](screenshots/2.png?raw=true "")
![](screenshots/3.png?raw=true "")

# Motivation
The initial motivation for creating this was exploring PS2 memory dumps and game files, specifically for finding clustered floating point numbers in Sly memory that could be part of interesting structures, like meshes or textures.
Despite the original use case, it can of course be used to visualize any file.
The floating point view in particular is very useful for game reverse engineering. To my knowledge there are not other tools that do this.
Cheat Engine has a similar feature, but it's very limited feature-wise and really slow since it works with RAM contents and has to update in real time.
