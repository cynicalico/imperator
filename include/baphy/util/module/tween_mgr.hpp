#ifndef BAPHY_UTIL_MODULE_TWEEN_MGR_HPP
#define BAPHY_UTIL_MODULE_TWEEN_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/util/rnd.hpp"
#include <functional>
#include <memory>
#include <numbers>
#include <string>
#include <unordered_map>

namespace baphy {

enum class Easing {
  linear,

  in_sine,
  out_sine,
  in_out_sine,

  in_quad,
  out_quad,
  in_out_quad,

  in_cubic,
  out_cubic,
  in_out_cubic,

  in_quart,
  out_quart,
  in_out_quart,

  in_quint,
  out_quint,
  in_out_quint,

  in_exp,
  out_exp,
  in_out_exp,

  in_circ,
  out_circ,
  in_out_circ,

  in_back,
  out_back,
  in_out_back,

  in_elastic,
  out_elastic,
  in_out_elastic,

  in_bounce,
  out_bounce,
  in_out_bounce
};

class TweenMgr : public Module<TweenMgr> {
public:
  TweenMgr() : Module<TweenMgr>() {}

  ~TweenMgr() override = default;

  template <typename T> requires std::convertible_to<T, double>
  std::string begin(T start, T end, double duration, Easing easing, std::function<void(double)> &&f) {
    return begin(rnd::base58(11), start, end, duration, easing, std::forward<std::function<void(double)>>(f));
  }

  template <typename T> requires std::convertible_to<T, double>
  std::string begin(std::string const &tag, T start, T end, double duration, Easing easing, std::function<void(double)> &&f) {
    tweens_[tag] = std::make_unique<Tween_<T>>(start, end, duration, easing, std::forward<std::function<void(double)>>(f));
    return tag;
  }

  void pause(const std::string &tag);
  void resume(const std::string &tag);
  void toggle(const std::string &tag);

private:
  struct TweenI_ {
  protected:
    static constexpr double c1 = 1.70158;
    static constexpr double c2 = c1 * 1.525;
    static constexpr double c3 = c1 + 1.0;
    static constexpr double c4 = (2.0 * std::numbers::pi) / 3.0;
    static constexpr double c5 = (2.0 * std::numbers::pi) / 4.5;

    static constexpr double n1 = 7.5625;
    static constexpr double d1 = 2.75;

    static std::unordered_map<Easing, std::function<double(double)>> easing_funcs_;
    Easing easing_;

    double duration_{0.0};
    double acc_{0.0};

  public:
    bool paused{false};

    TweenI_(double duration, const Easing &easing)
        : duration_(duration), easing_(easing) {}

    virtual ~TweenI_() = default;

    virtual bool update(double dt) = 0;
  };

  template<typename T> requires std::convertible_to<T, double>
  class Tween_ : public TweenI_ {
  private:
    double start_;
    double end_;
    std::function<void(double)> f_;

  public:
    Tween_(T start, T end, double duration, Easing easing, std::function<void(double)> &&f)
        : TweenI_(duration, easing),
          start_(static_cast<double>(start)),
          end_(static_cast<double>(end)),
          f_(std::forward<std::function<void(double)>>(f)) {}

    bool update(double dt) override {
      acc_ += dt;
      double p = acc_ / duration_;

      if (p < 1.0) {
        f_(start_ + (end_ - start_) * easing_funcs_[easing_](p));
        return false;
      } else {
        f_(end_);
        return true;
      }
    }
  };

  std::unordered_map<std::string, std::unique_ptr<TweenI_>> tweens_{};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::TweenMgr);

#endif//BAPHY_UTIL_MODULE_TWEEN_MGR_HPP
