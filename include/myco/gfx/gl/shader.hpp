#pragma once

#include "myco/util/rnd.hpp"
#include "myco/gfx/context2d.hpp"
#include "glad/gl.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace myco::gl {

struct ShaderSrc {
  std::optional<std::string> name;
  std::optional<std::string> vertex;
  std::optional<std::string> fragment;
};

class Shader {
public:
  GLuint id{0};
  std::string name{};

  explicit Shader(std::shared_ptr<Context2D> ctx, const ShaderSrc &src);
  ~Shader();

private:
  std::shared_ptr<Context2D> ctx_{nullptr};

  bool check_compile_(GLuint shader_id, GLenum type);
  bool check_link_();

  void gen_id_();
  void del_id_();
};

} // namespace myco::gl
