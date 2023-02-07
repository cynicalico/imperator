#include "myco/core/scheduler.hpp"

namespace myco {

std::vector<std::unordered_map<std::string, ReceiverI *>> Scheduler::receivers_{};

} // namespace myco
