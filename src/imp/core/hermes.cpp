#include "imp/core/hermes.hpp"

namespace imp {

std::vector<PrioList<Receiver>> Hermes::receivers_{};
std::vector<std::unordered_map<std::string, std::vector<std::any>>> Hermes::buffers_{};

std::recursive_mutex Hermes::receiver_mutex_;
std::recursive_mutex Hermes::buffer_mutex_;

} // namespace imp
