#ifndef IMPERATOR_GL_SHADER_H
#define IMPERATOR_GL_SHADER_H

#include "imperator/module/gfx/gfx_context.h"
#include "glm/glm.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace imp {
struct ShaderSrc {
    std::optional<std::string> name;
    std::optional<std::string> vertex;
    std::optional<std::string> fragment;

    std::string get() const;

    static std::optional<ShaderSrc> parse(const std::string &src);

    static std::optional<ShaderSrc> parse(const std::filesystem::path &path);
};

class Shader {
public:
    GladGLContext &gl;

    GLuint id{0};
    std::string name{};

    Shader(GfxContext &gfx, const ShaderSrc &src);

    ~Shader();

    // Copy constructors don't make sense for OpenGL objects
    Shader(const Shader &) = delete;

    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept;

    Shader &operator=(Shader &&other) noexcept;

    std::string src() const;

    void recompile(const ShaderSrc &src);

    void use();

    GLint get_attrib_loc(const std::string &attrib_name);

    void uniform_1f(const std::string &uniform_name, float v0);

    void uniform_2f(const std::string &uniform_name, float v0, float v1);

    void uniform_3f(const std::string &uniform_name, float v0, float v1, float v2);

    void uniform_4f(const std::string &uniform_name, float v0, float v1, float v2, float v3);

    void uniform_1f(const std::string &uniform_name, glm::vec1 v);

    void uniform_2f(const std::string &uniform_name, glm::vec2 v);

    void uniform_3f(const std::string &uniform_name, glm::vec3 v);

    void uniform_4f(const std::string &uniform_name, glm::vec4 v);

    void uniform_1i(const std::string &uniform_name, int v0);

    void uniform_2i(const std::string &uniform_name, int v0, int v1);

    void uniform_3i(const std::string &uniform_name, int v0, int v1, int v2);

    void uniform_4i(const std::string &uniform_name, int v0, int v1, int v2, int v3);

    void uniform_1i(const std::string &uniform_name, glm::ivec1 v);

    void uniform_2i(const std::string &uniform_name, glm::ivec2 v);

    void uniform_3i(const std::string &uniform_name, glm::ivec3 v);

    void uniform_4i(const std::string &uniform_name, glm::ivec4 v);

    void uniform_1u(const std::string &uniform_name, unsigned int v0);

    void uniform_2u(const std::string &uniform_name, unsigned int v0, unsigned int v1);

    void uniform_3u(const std::string &uniform_name, unsigned int v0, unsigned int v1, unsigned int v2);

    void
    uniform_4u(const std::string &uniform_name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3);

    void uniform_1u(const std::string &uniform_name, glm::uvec1 v);

    void uniform_2u(const std::string &uniform_name, glm::uvec2 v);

    void uniform_3u(const std::string &uniform_name, glm::uvec3 v);

    void uniform_4u(const std::string &uniform_name, glm::uvec4 v);

    void uniform_mat2f(const std::string &uniform_name, glm::mat2 v);

    void uniform_mat3f(const std::string &uniform_name, glm::mat3 v);

    void uniform_mat4f(const std::string &uniform_name, glm::mat4 v);

    void uniform_mat2x3f(const std::string &uniform_name, glm::mat2x3 v);

    void uniform_mat3x2f(const std::string &uniform_name, glm::mat3x2 v);

    void uniform_mat2x4f(const std::string &uniform_name, glm::mat2x4 v);

    void uniform_mat4x2f(const std::string &uniform_name, glm::mat4x2 v);

    void uniform_mat3x4f(const std::string &uniform_name, glm::mat3x4 v);

    void uniform_mat4x3f(const std::string &uniform_name, glm::mat4x3 v);

private:
    ShaderSrc src_;

    std::unordered_map<std::string, GLint> attrib_locs_{};

    std::unordered_map<std::string, GLint> uniform_locs_{};

    GLint get_uniform_loc_(const std::string &uniform_name);

    bool compile_shader_src_(const ShaderSrc &src);

    bool check_compile_(GLuint shader_id, GLenum type);

    bool check_link_();

    void gen_id_();

    void del_id_();

    void del_id_(GLuint id);
};
} // namespace imp

#endif//IMPERATOR_GL_SHADER_H
