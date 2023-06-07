#ifndef BAPHY_BAPHY_IMCONFIG_HPP
#define BAPHY_BAPHY_IMCONFIG_HPP

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#define IM_VEC2_CLASS_EXTRA                                      \
        constexpr ImVec2(const glm::vec2& f) : x(f.x), y(f.y) {} \
        operator glm::vec2() const { return glm::vec2(x, y); }

#define IM_VEC4_CLASS_EXTRA                                                      \
        constexpr ImVec4(const glm::vec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {} \
        operator glm::vec4() const { return glm::vec4(x, y, z, w); }

#endif//BAPHY_BAPHY_IMCONFIG_HPP
