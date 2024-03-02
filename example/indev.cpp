#include "imp/imp.h"
#include "fmt/format.h"

int main(int, char* []) {
  fmt::println("2 + 2 = {}", add(2, 2));
}
