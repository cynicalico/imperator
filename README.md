# myco

![Build](https://github.com/bo-fey/myco/actions/workflows/main.yml/badge.svg)

Myco is a WIP 2D graphics engine for C++ built on top of OpenGL.<br>
Primarily a passion project/fun side project to work on.

### Task list
- [x] EventBus
  - [x] PrioList 
- [ ] Windowing
  - [x] Position
  - [x] Size
  - [x] Decoration
  - [x] Icon
  - [x] vsync
  - [ ] User callbacks for events
- [ ] Timers
- [ ] [Tweens](https://easings.net/) 
- [ ] Thread pool
- [ ] Refactor out circular dependency on engine
  - [ ] Static module storage, maybe? Drawbacks but arguably better than circ dep
- [ ] User input
  - [ ] Keyboard
  - [ ] Mouse
  - [ ] Joystick (?)
  - [ ] User callbacks for events (as opposed to polling, which I prefer but isn't universal)
- [ ] Imgui integration
  - [ ] Add Implot
  - [ ] Debug overlay
    - [ ] FPS graph
    - [ ] FPS stats (1%/95%/99%)
    - [ ] Memusage
    - [ ] GPU usage (?)
  - [ ] In-window informatikon
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
