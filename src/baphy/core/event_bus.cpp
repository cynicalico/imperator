#include "baphy/core/event_bus.hpp"

namespace baphy {

std::vector<PrioList<std::unique_ptr<ReceiverI>>> EventBus::receivers_{};

} // namespace baphy
