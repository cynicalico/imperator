# baphy

[![build](https://github.com/beauregarde-f/baphy/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/beauregarde-f/baphy/actions/workflows/c-cpp.yml)

Baphy is a WIP 2D graphics engine for C++ built on top of OpenGL.<br>
Primarily a passion project/fun side project to work on.

### Task list

- [x] EventBus
    - [x] PrioList
- [x] Engine
- [ ] Windowing
    - [ ] Position
        - [x] Base impl
        - [ ] Handle different monitors
    - [x] Size
    - [x] Minimize
    - [x] Maximize
    - [x] Restore
    - [x] Resize
    - [x] Decoration
    - [x] Auto iconify
    - [x] Floating
    - [ ] Transparent
    - [x] Focus on show
    - [x] Icon
    - [ ] User callbacks for events
- [x] OpenGL context
    - [x] Creation
    - [x] vsync set/get
- [x] Timers
- [ ] [Tweens](https://easings.net/)
- [ ] Thread pool
- [x] Refactor out circular dependency on engine
    - [x] ~~Static module storage, maybe? Drawbacks but arguably better than circ dep~~
    - [x] ModuleMgr!
- [ ] User input
    - [x] Keyboard
    - [x] Mouse
    - [ ] Joystick (?)
    - [ ] User callbacks for events (as opposed to polling, which I prefer but isn't universal)
- [ ] Imgui integration
    - [x] Create module, initialize
    - [x] Add Implot
    - [ ] Debug overlay
        - [x] FPS
        - [x] FPS graph
        - [ ] FPS stats (1%/95%/99%)
        - [x] Memusage
        - [ ] GPU usage (?)
        - [ ] Control window
          - [x] Window
          - [x] GfxContext
          - [ ] InputMgr
    - [ ] In-window information
        - [x] Log
          - [x] Color
          - [x] All log events (including before creation/registration)
          - [ ] Configurable dock position (corners)
        - [ ] Stickies (track variables)
    - [x] Shader editor/recompiler
    - [ ] Batcher stats/information
- [ ] OpenGL abstraction
    - [x] Shaders
    - [x] Vertex buffers
    - [x] Vertex array objects
    - [ ] Framebuffers
- [ ] Batching system
- [ ] Primitives
    - [ ] Points
    - [ ] Lines
    - [ ] Triangles
    - [ ] Rectangles
    - [ ] Ovals
- [ ] Textures/sprites
    - [ ] Single
    - [ ] Sheets
        - [ ] Animation
- [ ] Particle system
- [ ] Audio
