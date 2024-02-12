#ifndef IMP_CORE_HERMES_HPP
#define IMP_CORE_HERMES_HPP

#include "imp/core/hermes_payloads.hpp"
#include "imp/core/prio_list.hpp"
#include "imp/core/type_id.hpp"
#include "imp/util/log.hpp"
#include "imp/util/map_macro.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace imp {
template<typename T>
struct EPI;
} // namespace imp

// Specialize EPI with the name of the module for use in the event system
// This is intended to be used outside of a namespace so the namespace becomes part
// of the name (just in case it is what qualifies it from another module)
#define IMP_PRAISE_HERMES(module)         \
  template<> struct imp::EPI<module> {    \
    static constexpr auto name = #module; \
  }

namespace imp {
class Hermes {
public:
  template<typename T>
  using Receiver = std::function<void(const T&)>;

  template<typename T>
  static void presub_cache(const std::string& name);

  template<typename T>
  static void sub(const std::string& name, std::vector<std::string>&& deps, Receiver<T>&& recv);

  template<typename T>
  static void sub(const std::string& name, Receiver<T>&& recv);

  template<typename T, typename... Args>
  static void send(Args&&... args);

  template<typename T, typename... Args>
  static void send_nowait(Args&&... args);

  // Call the receivers in reverse-dependency order
  // Useful if you need things to shut down correctly
  template<typename T, typename... Args>
  static void send_nowait_rev(Args&&... args);

  template<typename T>
  static void poll(const std::string& name);

  template<typename T>
  static std::vector<std::string> get_prio();

  template<typename T>
  static bool has_pending();

  template<typename T>
  static std::vector<PendingItemInfo> get_pending();

private:
  template<typename T>
  inline static PrioList<Receiver<T>> receivers_{};

  template<typename T>
  inline static std::unordered_map<std::string, std::vector<T>> buffers_{};

  inline static std::recursive_mutex receiver_mutex_;
  inline static std::recursive_mutex buffer_mutex_;

  template<typename T>
  static void check_create_buffer_(const std::string& name);
};
} // namespace imp

#include "imp/core/hermes.ipp"

// Wrap a function in a lambda to any_cast the payload to the correct type
// See examples of intended usage in any modules that use the event system (Window is a good one)
#define IMP_MAKE_RECEIVER(T, f)  \
  [&](const T& payload) {        \
    f(payload);                  \
  }

// Helper for IMP_HERMES_SUB
#define IMP_EPI_NAMIFY(module) EPI<module>::name

// Sub to a given event with an optional set of dependencies
// Ex:
//   IMP_HERMES_SUB(E_EndFrame, module_name, r_end_frame_, Application);
//
// NOTE: Dependencies must be registered with IMP_PRAISE_HERMES for this to work
#define IMP_HERMES_SUB(T, name, f, ...) \
  Hermes::sub<T>(name, {MAP_LIST(IMP_EPI_NAMIFY __VA_OPT__(,) __VA_ARGS__)}, IMP_MAKE_RECEIVER(T, f));

// This does the same thing as IMP_HERMES_SUB but takes the deps as a vector list
// This mostly exists for the sake of consistency, it's only calling IMP_MAKE_RECEIVER for you
// and otherwise is nearly equivalent to directly calling Hermes::sub
#define IMP_HERMES_SUB_VDEPS(T, name, f, deps) \
  Hermes::sub<T>(name, deps, IMP_MAKE_RECEIVER(T, f));

#endif//IMP_CORE_HERMES_HPP
