#define MYCO_INCLUDE_GL_WRAPPERS
#include "myco/myco.hpp"

class Indev : public myco::Application {
public:
  std::unique_ptr<myco::gl::Shader> shader{nullptr};
  myco::HSV clear_color = myco::hsv(0, 1, 1);

  void initialize() override {
    application_sticky("cc", clear_color);
    application_sticky("mx", input->mouse.x);
    application_sticky("my", "{:10}", input->mouse.y);

    shader = std::make_unique<myco::gl::Shader>(ctx, myco::gl::ShaderSrc::parse_src(R"(
#pragma name(test)

#pragma vertex
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

out vec4 frag_color;

void main() {
    frag_color = color;
    gl_Position = vec4(pos, 1.0);
}

#pragma fragment
#version 330 core
in vec4 frag_color;

out vec4 FragColor;

void main() {
    FragColor = frag_color;
}
)"));

    application_show_debug_overlay();
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);

    if (input->pressed("f3"))
      application_toggle_debug_overlay();

    clear_color.h = std::fmod(clear_color.h + (10 * dt), 360.0);
  }

  void draw() override {
    ctx->clear(clear_color.to_rgb());
  }
};

MYCO_RUN(Indev, (myco::WindowOpenParams{
    .title = "Indev",
    .size = {500, 500},
    .flags = myco::WindowFlags::centered
}))
