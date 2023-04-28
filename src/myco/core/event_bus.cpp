#include "myco/core/event_bus.h"

namespace myco {

std::vector<PrioList<std::unique_ptr<ReceiverI>>> EventBus::receivers_{};

} // namespace myco
