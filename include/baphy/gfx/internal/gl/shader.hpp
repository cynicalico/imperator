#ifndef BAPHY_GFX_INTERNAL_GL_SHADER_HPP
#define BAPHY_GFX_INTERNAL_GL_SHADER_HPP

#include "baphy/gfx/module/gfx_context.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace baphy {

struct ShaderSrc {
  std::optional<std::string> name;
  std::optional<std::string> vertex;
  std::optional<std::string> fragment;

  static std::optional<ShaderSrc> parse(const std::string &src);
  static std::optional<ShaderSrc> parse(const std::filesystem::path &path);
};

class Shader {
public:
  std::shared_ptr<GfxContext> gfx{nullptr};

  GLuint id{0};
  std::string name{};

  Shader(std::shared_ptr<GfxContext> gfx, const ShaderSrc &src);

  ~Shader();

private:
  void compile_shader_src_(const ShaderSrc &src);

  bool check_compile_(GLuint shader_id, GLenum type);
  bool check_link_();

  void gen_id_();
  void del_id_();
};

} // namespace baphy

#endif//BAPHY_GFX_INTERNAL_GL_SHADER_HPP
