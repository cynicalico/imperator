#include "baphy/core/event_bus.hpp"

namespace baphy {

std::vector<PrioList<std::unique_ptr<ReceiverI>>> EventBus::receivers_{};
std::vector<std::unordered_map<std::string, std::vector<std::unique_ptr<EBase>>>> EventBus::buffers_{};

} // namespace baphy
