#include "fmt/format.h"
#include "re2/re2.h"
#include <cassert>
#include <string>

int main(int, char *[]) {
  std::string s = "hello how are you 123";

  RE2 re(R"(([a-z]+))");
  assert(re.ok());

  auto sp = re2::StringPiece(s);
  std::string w;
  while (RE2::FindAndConsume(&sp, re, &w)) {
    fmt::println("{}", w);
  }
}
