#include "fmt/format.h"
#include "re2/re2.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <optional>
#include <streambuf>
#include <string>

const auto SHADERS = std::filesystem::path(__FILE__).parent_path() / "shaders";

std::string read_file(const std::filesystem::path &path) {
  std::ifstream t(path);
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());

  return str;
}

struct ShaderSrc {
  std::string name{"unknown"};
  std::optional<std::string> vert{};
  std::optional<std::string> frag{};
  std::optional<std::string> geom{};

  void add(const std::string &type, const std::string &src) {
    if (type == "vertex")
      vert = src;
    else if (type == "fragment")
      frag = src;
    else if (type == "geometry")
      geom = src;
    else
      throw std::exception(fmt::format("Unknown shader pragma: {}", type).c_str());
  }
};

int main(int, char *[]) {
  RE2 pragma_w_args(R"(#pragma (.+)\((.*)\)\n)");
  assert(pragma_w_args.ok());

  RE2 pragma_no_args(R"(#pragma (.+)\n)");
  assert(pragma_no_args.ok());

  auto src = ShaderSrc{};

  auto s = read_file(SHADERS / "basic.shader");
  auto sp = re2::StringPiece(s);

  std::string c1, c2;
  std::vector<std::tuple<std::string, std::string, std::size_t>> pragmas{};

  while (RE2::FindAndConsume(&sp, pragma_w_args, &c1, &c2)) {
    auto curr_pos = sp.data() - s.data();
    fmt::println("{} {} {}", c1, c2, curr_pos);
    pragmas.emplace_back(c1, c2, curr_pos);
  }

  while (RE2::FindAndConsume(&sp, pragma_no_args, &c1)) {
    auto curr_pos = sp.data() - s.data();
    fmt::println("{} {}", c1, curr_pos);
    pragmas.emplace_back(c1, c1, curr_pos);
  }

  for (int i = 0; i < pragmas.size(); ++i) {
    auto &[name, value, n] = pragmas[i];

    if (name == "name")
      src.name = value;

    else
      src.add(
          name,
          (i < pragmas.size() - 1)
              ? s.substr(n, std::get<2>(pragmas[i+1]) - n)
              : s.substr(n, std::string::npos)
      );
  }

  fmt::println("vert:\n{}", src.vert.value_or("none"));
  fmt::println("frag:\n{}", src.frag.value_or("none"));
  fmt::println("geom:\n{}", src.geom.value_or("none"));
}
