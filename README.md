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
    - [ ] Icon
    - [ ] User callbacks for events
- [ ] OpenGL context
  - [ ] vsync
- [ ] Timers
- [ ] [Tweens](https://easings.net/)
- [ ] Thread pool
- [x] Refactor out circular dependency on engine
    - [x] ~~Static module storage, maybe? Drawbacks but arguably better than circ dep~~
    - [x] ModuleMgr!
- [ ] User input
    - [ ] Keyboard
    - [ ] Mouse
    - [ ] Joystick (?)
    - [ ] User callbacks for events (as opposed to polling, which I prefer but isn't universal)
- [ ] Imgui integration
  - [x] Create module, initialize
  - [ ] Add Implot
  - [ ] Debug overlay
      - [ ] FPS graph
      - [ ] FPS stats (1%/95%/99%)
      - [ ] Memusage
      - [ ] GPU usage (?)
  - [ ] In-window information
      - [ ] Log
      - [ ] Stickies (track variables)
  - [ ] Shader editor/recompiler
  - [ ] Batcher stats/information
- [ ] OpenGL abstraction
    - [ ] Shaders
    - [ ] Vertex buffers
    - [ ] Vertex array objects
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
