#include "imperator/util/module/debug_overlay.hpp"

#include "imperator/core/module/application.hpp"
#include "imperator/core/module/input_mgr.hpp"
#include "imperator/gfx/module/gfx_context.hpp"
#include "imperator/util/io.hpp"
#include "imperator/util/memusage.hpp"
#include "imperator/util/sops.hpp"
#include <fstream>

#include "imperator/core/module/glfw_callbacks.hpp"

namespace imp {
// This should split a string into arguments like a command line does,
// including respecting escaped quotes
// TODO: Make this into a parser rather than regex
std::vector<std::string> argument_split(const std::string& s) {
  static RE2 pat(R"(([^\s"]+|\"(?:[\w\s]|(?:\\\"))*?\"))");
  // assert(pat.ok());

  std::vector<std::string> args{};

  std::string_view sp(s);
  std::string arg;
  while (RE2::FindAndConsume(&sp, pat, &arg)) {
    args.emplace_back(arg);
  }

  return args;
}

void DebugOverlay::set_flying_log_enabled(bool enabled) {
  flying_log_.enabled = enabled;
}

bool DebugOverlay::is_flying_log_enabled() const {
  return flying_log_.enabled;
}

void DebugOverlay::set_flying_log_limit(std::size_t max_lines) {
  flying_log_.max_lines = max_lines;
}

void DebugOverlay::set_flying_log_timeout(double timeout) {
  flying_log_.timeout = timeout;
}

void DebugOverlay::register_console_cmd(
  const std::string& name,
  const ConsoleParseFunc&& parser_setup,
  const ConsoleCallbackFunc&& callback
) {
  argparse::ArgumentParser p(name, "1.0", argparse::default_arguments::none);
  parser_setup(p);

  std::string usage = p.usage();
  if (usage.starts_with("Usage: ")) {
    usage = usage.substr(7);
  }

  console_.trie.insert(name, usage);
  console_.parsers[name] = std::forward<const ConsoleParseFunc>(parser_setup);
  console_.callbacks[name] = std::forward<const ConsoleCallbackFunc>(callback);
}

void DebugOverlay::set_console_binding(const std::string& binding) {
  console_.binding = binding;
}

void DebugOverlay::r_initialize_(const E_Initialize& p) {
  Hermes::sub<E_Draw>(module_name, {EPI<Application>::name}, [&](const auto& p) { r_draw_(p); });
  Hermes::sub<E_Update>(module_name, [&](const auto& p) { r_update_(p); });
  Hermes::sub<E_LogMsg>(module_name, [&](const auto& p) { r_log_msg_(p); });
  Hermes::sub<E_GlfwWindowSize>(module_name, [&](const auto& p) { r_glfw_window_size_(p); });

  Module::r_initialize_(p);
}

void DebugOverlay::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}

void DebugOverlay::r_update_(const E_Update& p) {
  // Late-initialization of required modules
  if (!inputs) { inputs = module_mgr->get<InputMgr>(); }
  if (!gfx) { gfx = module_mgr->get<GfxContext>(); }

  fps = p.fps;

  for (auto& [s, color, acc, fade_max]: flying_log_.lines) {
    acc -= p.dt;
  }
  while (!flying_log_.lines.empty() && flying_log_.lines.back().acc <= 0.0) {
    flying_log_.lines.pop_back();
  }
  Hermes::poll<E_LogMsg>(module_name);

  if (inputs->pressed(console_.binding)) {
    console_.enabled = true;
    set_ignore_imgui_capture(inputs->get_glfw_actions(console_.binding), GLFW_RELEASE);
  }
}

void DebugOverlay::r_draw_(const E_Draw& p) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {1, 1});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0, 0});
  ImGui::SetNextWindowPos({WINDOW_EDGE_PADDING, WINDOW_EDGE_PADDING});
  if (ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
    const auto vsync_str = gfx->is_vsync() ? " (vsync)" : "";
    ImGui::Text("%s", fmt::format("{:.2f} fps{}{}", fps, vsync_str, BUILD_TYPE).c_str());
    ImGui::Text("%s", fmt::format("{:.2f} MB", imp::memusage_mb()).c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar(2);

  if (!tabs.empty()) {
    if (ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      static std::string current_item{tabs.begin()->first};
      if (ImGui::BeginCombo("##combo", current_item.c_str())) {
        for (const auto& k: tabs | std::views::keys) {
          const bool is_selected = current_item == k;
          if (ImGui::Selectable(k.c_str(), is_selected)) {
            current_item = k;
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      ImGui::Separator();
      tabs[current_item]();
    }
    ImGui::End();
  }

  if (console_.enabled) {
    auto desired_width = 3.0f * (window_size_.x / 4.0f);
    auto max_height = window_size_.y * 0.9f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {2, 2});
    ImGui::SetNextWindowPos({(window_size_.x / 4.0f) / 2.0f, WINDOW_EDGE_PADDING});
    ImGui::SetNextWindowSizeConstraints({desired_width, 0.0f}, {desired_width, max_height});
    if (ImGui::Begin("Fuzzy matching", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration)) {
      ImGui::PushItemWidth(-1);
      if (ImGui::InputText("##console_input", &console_.input, ImGuiInputTextFlags_EnterReturnsTrue)) {
        const auto args = argument_split(console_.input);
        if (!args.empty()) {
          if (console_.parsers.contains(args[0])) {
            auto parser = argparse::ArgumentParser(args[0]);
            console_.parsers[args[0]](parser);
            try {
              parser.parse_args(args);
              console_.callbacks[args[0]](parser);

              console_.input.clear();
              console_.enabled = false;
            } catch (std::exception& e) {
              IMPERATOR_LOG_ERROR("Parse error: {}", e.what());
            }
          }
        }
      }
      if ((ImGui::IsWindowFocused() || !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) && !
          ImGui::IsAnyItemActive()) {
        ImGui::SetKeyboardFocusHere(-1);
      }
      ImGui::PopItemWidth();

      if (!console_.input.empty()) {
        ImGui::Separator();

        const auto pre = argument_split(console_.input);
        if (!pre.empty() && pre[0] != console_.last_pre) {
          console_.matches = console_.trie.fuzzy_match_n(pre[0]);
          console_.last_pre = pre[0];
        }

        if (ImGui::BeginChild("##predictions", {0, 0}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) {
          for (const auto& m: console_.matches) {
            ImGui::Text("%s", m.second->c_str());
          }
        }
        ImGui::EndChild();
      }
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

  auto dl = ImGui::GetBackgroundDrawList();
  auto pos = glm::vec2{WINDOW_EDGE_PADDING, window_size_.y - WINDOW_EDGE_PADDING};

  auto draw_line = [&](const std::string& s, const RGB& color, float a) {
    auto text_size = ImGui::CalcTextSize(s.c_str());
    dl->AddRectFilled(
      {pos.x, pos.y - text_size.y - 2},
      {pos.x + text_size.x + 2, pos.y},
      ImGui::GetColorU32(ImGuiCol_WindowBg, a)
    );
    dl->AddText(
      {pos.x + 1, pos.y - text_size.y - 1},
      ImGui::GetColorU32(color.vec4()),
      s.c_str()
    );
    pos.y -= text_size.y + 2;
  };

  for (const auto& [s, color, acc, fade_max]: flying_log_.lines) {
    float a = 1.0f;
    if (acc <= fade_max) {
      a = acc / fade_max;
    }
    auto c = color;
    c.a = c.a * a;
    draw_line(s, c, a);
  }
}

void DebugOverlay::r_log_msg_(const E_LogMsg& p) {
  // TODO: Make these customizable
  static std::unordered_map<spdlog::level::level_enum, RGB> level_colors{
    {spdlog::level::trace, rgb(0x808080)},
    {spdlog::level::debug, rgb(0x8080ff)},
    {spdlog::level::info, rgba(ImGui::GetColorU32(ImGuiCol_Text), true)},
    {spdlog::level::warn, rgb(0xffff80)},
    {spdlog::level::err, rgb(0xff8080)},
    {spdlog::level::critical, rgb(0xff0000)}
  };

  if (flying_log_.lines.size() == flying_log_.max_lines) {
    flying_log_.lines.pop_back();
  }
  flying_log_.lines.emplace_front(
    p.text,
    level_colors[p.level],
    flying_log_.timeout,
    flying_log_.timeout / 2.0
  );
}

void DebugOverlay::r_glfw_window_size_(const E_GlfwWindowSize& p) {
  window_size_.x = p.width;
  window_size_.y = p.height;
}
} // namespace imp
