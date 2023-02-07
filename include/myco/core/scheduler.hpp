#pragma once

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace myco {

inline int type_id_seq = 0;
template<typename T> inline const int type_id = type_id_seq++;

template<typename T>
using Receiver = std::function<void(const T &)>;

class ReceiverI {
public:
  virtual void call(std::any e) const = 0;
};

template<typename T>
class ReceiverWrap : public ReceiverI {
  const Receiver<T> receiver_;
public:
  ReceiverWrap(const Receiver<T> &receiver)
      : receiver_(std::move(receiver)) {}

  void call(std::any e) const override {
    receiver_(std::any_cast<T>(e));
  }
};

class Scheduler {
public:
  template<typename T>
  static void sub(const std::string &name, Receiver<T> recv) {
    if (type_id<T> >= receivers_.size())
      receivers_.push_back({{name, new ReceiverWrap<T>(recv)}});
    else
      receivers_[type_id<T>][name] = new ReceiverWrap<T>(recv);
  }

  template<typename T, typename... Args>
  static void send(Args &&... args) {
    for (const auto &p : receivers_[type_id<T>])
      p.second->call(std::any(T{std::forward<Args>(args)...}));
  }

private:
  static std::vector<std::unordered_map<std::string, ReceiverI *>> receivers_;
};

} // namespace myco
