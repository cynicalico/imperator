:warning: ***This is undergoing a big rewrite (and name change) at the moment, so I'm working on the redux branch. No ETA, but progress is being made!*** :warning:

# baphy

[![build](https://github.com/bogicide/baphy/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/bogicide/baphy/actions/workflows/c-cpp.yml)

Baphy is a WIP 2D graphics engine for C++ built on top of OpenGL.<br>
Primarily a passion project/fun side project to work on.

### Motivation

There are already several existing projects that do a lot of the same things as Baphy, so why make another one?
1. Many existing projects such as [SDL2](https://www.libsdl.com) have very C-like interfaces. I wanted to create something that took advantage of more modern C++ features.
2. Projects like [bgfx](https://www.github.com/bkaradzic/bgfx) appear to use old-style OpenGL (pre 3.0) behind the scenes--if this is incorrect, someone correct me!
3. For fun! This is frankly the most important one--even if the other two points were completely invalidated I would still want to make Baphy.

### Features

Baphy has a lot it can do, and a lot it can't. Features are added as I decide to, not necessarily in any particular order of importance. <br>

- [ ] Windowing*
- [ ] User input*
- [ ] Primitives*
- [x] Textures
- [x] Off-screen surfaces
- [ ] Particle systems based on the [LÖVE2D implementation](https://love2d.org/wiki/ParticleSystem)* 
- [ ] Audio*
- [x] Timers
- [x] Tweens
- [x] Cursors
- [x] Simple RNG interface from [PCG](https://github.com/imneme/pcg-cpp)
- [x] [Various utilities](https://github.com/bofehlan/baphy/tree/main/include/baphy/util)

*partial support or WIP feature, check the issues

### Status

See the Project board for more information.

### Contributing

This may change in the future but for now I'd like to keep all code written by myself as a showcase of my own skill.

Baphy is a labor of love for me, and acts partially as a portfolio project. Please do not pull request as I won't accept it. <br>
If there is a particular feature you'd like implemented, feel free to create an issue--no promises I will get to it quickly but if I agree it's within scope I will try and get to it eventually.

### Credits

I'm using several great open-source projects to create Baphy.

- [{fmt}](https://github.com/fmtlib/fmt)
- [glad](https://github.com/Dav1dde/glad)
- [GLFW](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [Dear ImGui](https://github.com/ocornut/imgui/tree/docking)
- [ImPlot](https://github.com/epezent/implot)
- [json](https://github.com/nlohmann/json)
- [Libnyquist](https://github.com/ddiakopoulos/libnyquist)
- [Magic Enum C++](https://github.com/Neargye/magic_enum)
- [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended)
- [OpenAL Soft](https://github.com/kcat/openal-soft)
- [PCG](https://github.com/imneme/pcg-cpp)
- [range-v3](https://github.com/ericniebler/range-v3)
- [Rapidcsv](https://github.com/d99kris/rapidcsv)
- [RE2](https://github.com/google/re2)
- [spdlog](https://github.com/gabime/spdlog)
- [stb](https://github.com/nothings/stb)
- [stduuid](https://github.com/mariusbancila/stduuid)
