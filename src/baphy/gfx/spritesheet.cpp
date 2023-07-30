#include <utility>

#include "baphy/gfx/spritesheet.hpp"

#include "baphy/util/io.hpp"

namespace baphy {

struct Rect { int x, y, w, h; };

void from_json(const nlohmann::json &j, Rect &p) {
  j.at("x").get_to(p.x);
  j.at("y").get_to(p.y);
  j.at("w").get_to(p.w);
  j.at("h").get_to(p.h);
}

Spritesheet::Spritesheet(std::shared_ptr<Texture> tex, const std::filesystem::path &path_to_spec) : tex_(std::move(tex)) {
  auto spec = read_json(path_to_spec);
  if (spec) {
    auto frames = json_get<nlohmann::json>(*spec, "frames");
    if (!frames) {
      BAPHY_LOG_ERROR("No such key 'frames' in spritesheet spec; check Aseprite export");
      return;
    } else if (!frames->is_array()) {
      BAPHY_LOG_ERROR("Frames are not formatted as array; check Aseprite export");
      return;
    }
    auto meta = json_get<nlohmann::json>(*spec, "meta");
    if (!meta) {
      BAPHY_LOG_ERROR("No such key 'meta' in spritesheet spec; check Aseprite export");
      return;
    }
    auto slices = json_get<nlohmann::json>(*meta, "slices");
    if (!slices) {
      BAPHY_LOG_ERROR("No slices in meta info; check Aseprite export");
      return;
    }

    for (std::size_t i = 0; i < slices->size(); ++i) {
      auto name = json_get<std::string>(slices->at(i), "name");
      auto keys = (*json_get<nlohmann::json>(slices->at(i), "keys"))[0];

      auto frame = json_get<std::size_t>(keys, "frame");
      auto frame_pos = (*frames)[*frame]["frame"].template get<Rect>();

      auto bounds = keys["bounds"].template get<Rect>();

      regions_[*name] = {
          static_cast<float>(frame_pos.x + bounds.x),
          static_cast<float>(frame_pos.y + bounds.y),
          static_cast<float>(bounds.w),
          static_cast<float>(bounds.h)
      };
    }
  }
}

void Spritesheet::set_scale(float s) {
  scale_ = s;
}

float Spritesheet::w(const std::string &sprite_name) {
  return regions_[sprite_name][2] * scale_;
}

float Spritesheet::h(const std::string &sprite_name) {
  return regions_[sprite_name][3] * scale_;
}

void Spritesheet::draw(const std::string &sprite_name, float x, float y, float rx, float ry, float angle, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, r[2] * scale_, r[3] * scale_, r[0], r[1], r[2], r[3], rx, ry, angle, color);
}

void Spritesheet::draw(const std::string &sprite_name, float x, float y, float angle, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, r[2] * scale_, r[3] * scale_, r[0], r[1], r[2], r[3], x + (r[2] * scale_) / 2, y + (r[3] * scale_) / 2, angle, color);
}

void Spritesheet::draw(const std::string &sprite_name, float x, float y, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, r[2] * scale_, r[3] * scale_, r[0], r[1], r[2], r[3], 0, 0, 0, color);
}

void Spritesheet::draw_scale(const std::string &sprite_name, float x, float y, float scale, float rx, float ry, float angle, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, r[2] * scale, r[3] * scale, r[0], r[1], r[2], r[3], rx, ry, angle, color);
}

void Spritesheet::draw_scale(const std::string &sprite_name, float x, float y, float scale, float angle, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, r[2] * scale, r[3] * scale, r[0], r[1], r[2], r[3], x + (r[2] * scale) / 2, y + (r[3] * scale) / 2, angle, color);
}

void Spritesheet::draw_scale(const std::string &sprite_name, float x, float y, float scale, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, r[2] * scale, r[3] * scale, r[0], r[1], r[2], r[3], 0, 0, 0, color);
}

void Spritesheet::draw_wh(const std::string &sprite_name, float x, float y, float w, float h, float rx, float ry, float angle, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, w, h, r[0], r[1], r[2], r[3], rx, ry, angle, color);
}

void Spritesheet::draw_wh(const std::string &sprite_name, float x, float y, float w, float h, float angle, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, w, h, r[0], r[1], r[2], r[3], x + w / 2, y + h / 2, angle, color);
}

void Spritesheet::draw_wh(const std::string &sprite_name, float x, float y, float w, float h, const RGB &color) {
  auto r = regions_[sprite_name];
  tex_->draw(x, y, w, h, r[0], r[1], r[2], r[3], 0, 0, 0, color);
}

} // namespace baphy
