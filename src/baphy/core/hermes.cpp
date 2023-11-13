#include "baphy/core/hermes.hpp"

namespace baphy {

std::vector<PrioList<std::unique_ptr<ReceiverI>>> Hermes::receivers_{};
std::vector<std::unordered_map<std::string, std::vector<std::any>>> Hermes::buffers_{};

} // namespace baphy
