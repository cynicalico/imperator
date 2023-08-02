#include "baphy/util/module/tween_mgr.hpp"

namespace baphy {

void TweenMgr::pause(const std::string &tag) {
  auto it = tweens_.find(tag);
  if (it != tweens_.end())
    it->second->paused = true;
}

void TweenMgr::resume(const std::string &tag) {
  auto it = tweens_.find(tag);
  if (it != tweens_.end())
    it->second->paused = false;
}

void TweenMgr::toggle(const std::string &tag) {
  auto it = tweens_.find(tag);
  if (it != tweens_.end())
    it->second->paused = !it->second->paused;
}

bool TweenMgr::finished(const std::string &tag) {
  return finished_tweens_.contains(tag);
}

void TweenMgr::e_initialize_(const baphy::EInitialize &e) {
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });

  Module::e_initialize_(e);
}

void TweenMgr::e_shutdown_(const baphy::EShutdown &e) {
  Module::e_shutdown_(e);
}

void TweenMgr::e_update_(const baphy::EUpdate &e) {
  finished_tweens_.clear();

  for (auto it = tweens_.begin(); it != tweens_.end(); ) {
    auto &tween = it->second;

    if (!tween->paused)
      if (tween->update(e.dt)) {
        finished_tweens_.insert(it->first);
        it = tweens_.erase(it);
        continue;
      }

    it++;
  }
}

std::unordered_map<Easing, std::function<double(double)>> TweenMgr::TweenI_::easing_funcs_ = {
    {Easing::linear, [](double p) -> double {
      return p;
    }},

    {Easing::in_sine, [](double p) -> double {
      return 1 - std::cos((p * std::numbers::pi) / 2.0);
    }},
    {Easing::out_sine, [](double p) -> double {
      return std::sin((p * std::numbers::pi) / 2.0);
    }},
    {Easing::in_out_sine, [](double p) -> double {
      return -(std::cos(p * std::numbers::pi) - 1) / 2.0;
    }},

    {Easing::in_quad, [](double p) -> double {
      return p * p;
    }},
    {Easing::out_quad, [](double p) -> double {
      return 1 - (1 - p) * (1 - p);
    }},
    {Easing::in_out_quad, [](double p) -> double {
      return p < 0.5
             ? 2 * p * p
             : 1 - std::pow(-2 * p + 2, 2) / 2;
    }},

    {Easing::in_cubic, [](double p) -> double {
      return p * p * p;
    }},
    {Easing::out_cubic, [](double p) -> double {
      return 1 - std::pow(1 - p, 3);
    }},
    {Easing::in_out_cubic, [](double p) -> double {
      return p < 0.5
             ? 4 * p * p * p
             : 1 - std::pow(-2 * p + 2, 3) / 2;
    }},

    {Easing::in_quart, [](double p) -> double {
      return p * p * p * p;
    }},
    {Easing::out_quart, [](double p) -> double {
      return 1 - std::pow(1 - p, 4);
    }},
    {Easing::in_out_quart, [](double p) -> double {
      return p < 0.5
             ? 8 * p * p * p * p
             : 1 - std::pow(-2 * p + 2, 4) / 2;
    }},

    {Easing::in_quint, [](double p) -> double {
      return p * p * p * p * p;
    }},
    {Easing::out_quint, [](double p) -> double {
      return 1 - std::pow(1 - p, 5);
    }},
    {Easing::in_out_quint, [](double p) -> double {
      return p < 0.5
             ? 16 * p * p * p * p * p
             : 1 - std::pow(-2 * p + 2, 5) / 2;
    }},

    {Easing::in_exp, [](double p) -> double {
      return p == 0.0 ? 0.0 : std::pow(2, 10 * p - 10);
    }},
    {Easing::out_exp, [](double p) -> double {
      return p == 1.0 ? 1.0 : 1 - std::pow(2, -10 * p);
    }},
    {Easing::in_out_exp, [](double p) -> double {
      return p == 0.0
             ? 0.0
             : p == 1.0
               ? 1.0
               : p < 0.5
                 ? std::pow(2, 20 * p - 10) / 2
                 : (2 - std::pow(2, -20 * p + 10)) / 2;
    }},

    {Easing::in_circ, [](double p) -> double {
      return 1 - std::sqrt(1 - std::pow(p, 2));
    }},
    {Easing::out_circ, [](double p) -> double {
      return std::sqrt(1 - std::pow(p - 1, 2));
    }},
    {Easing::in_out_circ, [](double p) -> double {
      return p < 0.5
             ? (1 - std::sqrt(1 - std::pow(2 * p, 2))) / 2
             : (std::sqrt(1 - std::pow(-2 * p + 2, 2)) + 1) / 2;
    }},

    {Easing::in_back, [](double p) -> double {
      return c3 * p * p * p - c1 * p * p;
    }},
    {Easing::out_back, [](double p) -> double {
      return 1 + c3 * std::pow(p - 1, 3) + c1 * std::pow(p - 1, 2);
    }},
    {Easing::in_out_back, [](double p) -> double {
      return p < 0.5
             ? (std::pow(2 * p, 2) * ((c2 + 1) * 2 * p - c2)) / 2
             : (std::pow(2 * p - 2, 2) * ((c2 + 1) * (p * 2 - 2) + c2) + 2) / 2;
    }},

    {Easing::in_elastic, [](double p) -> double {
      return p == 0.0
             ? 0.0
             : p == 1.0
               ? 1.0
               : -std::pow(2, 10 * p - 10) * std::sin((p * 10 - 10.75) * c4);
    }},
    {Easing::out_elastic, [](double p) -> double {
      return p == 0.0
             ? 0.0
             : p == 1.0
               ? 1.0
               : std::pow(2, -10 * p) * std::sin((p * 10 - 0.75) * c4) + 1;
    }},
    {Easing::in_out_elastic, [](double p) -> double {
      return p == 0.0
             ? 0.0
             : p == 1.0
               ? 1.0
               : p < 0.5
                 ? -(std::pow(2, 20 * p - 10) * std::sin((20 * p - 11.125) * c5)) / 2
                 : (std::pow(2, -20 * p + 10) * std::sin((20 * p - 11.125) * c5)) / 2 + 1;
    }},

    {Easing::in_bounce, [](double p) -> double {
      return 1 - easing_funcs_[Easing::out_bounce](1 - p);
    }},
    {Easing::out_bounce, [](double p) -> double {
      if (p < 1 / d1) {
        return n1 * p * p;
      } else if (p < 2 / d1) {
        p -= 1.5 / d1;
        return n1 * p * p + 0.75;
      } else if (p < 2.5 / d1) {
        p -= 2.25 / d1;
        return n1 * p * p + 0.9375;
      } else {
        p -= 2.625 / d1;
        return n1 * p * p + 0.984375;
      }
    }},
    {Easing::in_out_bounce, [](double p) -> double {
      return p < 0.5
             ? (1 - easing_funcs_[Easing::out_bounce](1 - 2 * p)) / 2
             : (1 + easing_funcs_[Easing::out_bounce](2 * p - 1)) / 2;
    }}
};

} // namespace baphy
