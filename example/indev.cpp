#include "baphy/util/rnd.hpp"
#include "baphy/util/log.hpp"

int main(int, char *[]) {
  BAPHY_LOG_INFO("hello {}", baphy::get<float>());
}