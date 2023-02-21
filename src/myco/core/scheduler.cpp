#include "myco/core/scheduler.hpp"

namespace myco {

std::vector<PrioList<std::unique_ptr<ReceiverI>>> Scheduler::receivers_{};

} // namespace myco
