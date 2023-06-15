#include "baphy/util/memusage.hpp"

#include "baphy/util/platform.hpp"

#if defined(BAPHY_PLATFORM_WINDOWS)
#include <Windows.h>
#include <Psapi.h>
#elif defined(BAPHY_PLATFORM_LINUX)
#include <cstdio>
#include <unistd.h>
#include <sys/resource.h>
#endif

namespace baphy {

double memusage_mb() {
#if defined(BAPHY_PLATFORM_WINDOWS)
  /* NOTE: This number will *not* match what you see in Task Manager, as it is the sum of
   *   the private memory usage (the running program) + the memory being used by any DLLs
   *   that are loaded for it. In my experience this makes the number 10-15 MB higher than
   *   reported in Task Manager, but it could be more or less.
   */
  PROCESS_MEMORY_COUNTERS_EX pmc;
  if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)))
    return pmc.WorkingSetSize / 1024.0 / 1024.0;
  else
    return 0.0;

#elif defined(BAPHY_PLATFORM_LINUX)
  /* NOTE: This number will *not* match what you see in Resource Monitor, as it is the
   *   resident memory size, not the amount of memory that the process is currently using.
   */
  long rss = 0L;
  FILE *fp = nullptr;
  if ((fp = fopen( "/proc/self/statm", "r" )) == nullptr)
    return (size_t)0L;      /* Can't open? */
  if (fscanf(fp, "%*s%ld", &rss) != 1) {
    fclose(fp);
    return (size_t)0L;      /* Can't read? */
  }
  fclose(fp);
  return ((size_t)rss * (size_t)sysconf(_SC_PAGESIZE)) / 1024.0 / 1024.0;
#endif
}

} // namespace baphy
