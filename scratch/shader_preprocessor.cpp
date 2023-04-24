#include "fmt/format.h"
#include "fmt/ranges.h"
#include "spdlog/spdlog.h"
#include "re2/re2.h"
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

auto HERE = std::filesystem::path(__FILE__).parent_path();

std::vector<std::string> split(const std::string &s, const std::string &delim) {
    std::vector<std::string> res{};

    std::size_t pos_start = 0, pos_end, delim_len = delim.length();
    std::string token;
    while ((pos_end = s.find(delim, pos_start)) != std::string::npos) {
        res.emplace_back(s.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
    }
    res.emplace_back(s.substr(pos_start));

    return res;
}

/**
 * Split a given string on a regular expression
 * regexp must be a regular expression of the form (...), where ... can be any valid re
 * If regexp does not have a match pattern, this will return an empty vector
 *
 * FIXME: This is likely very fragile and almost certainly will not work with anything but ASCII
 *  text--it is probably possible to make it work with generic UTF-8 but I'm already pushing the
 *  limits on how re2 works here
 *
 * @param s the string to split
 * @param regexp the regular expression to split on
 * @return a vector of substrings of s split on regexp
 */
std::vector<std::string> split(const std::string &s, const RE2 &regexp) {
    std::vector<std::string> res;

    re2::StringPiece sp(s);
    auto last_match_it = sp.begin();
    std::size_t pos = 0;

    std::string match;
    while (RE2::FindAndConsume(&sp, regexp, &match)) {
        auto len = std::distance(last_match_it, sp.begin()) - match.size();
        res.emplace_back(s.substr(pos, len));
        pos += std::distance(last_match_it, sp.begin());

        last_match_it = sp.begin();
    }
    res.emplace_back(s.substr(pos));

    return res;
}

struct ShaderSrc {
    std::string name{"noname"};
    std::optional<std::string> vertex{};
    std::optional<std::string> fragment{};
};

std::optional<std::string> read_file_process_includes(const std::filesystem::path &path) {
    const static RE2 spaces_pat{R"((\s+))"};
    assert(spaces_pat.ok());

    const static RE2 path_pat{R"("(.+)\")"};
    assert(path_pat.ok());

    std::ifstream ifs(path.string());
    if (!ifs.is_open()) {
        SPDLOG_ERROR("Failed to open file: '{}'", path.string());
        return {};
    }

    std::string s;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.starts_with("#include")) {
            auto tokens = split(line, spaces_pat);

            std::string include;
            if (tokens.size() < 2 || !RE2::FullMatch(tokens[1], path_pat, &include)) {
                SPDLOG_ERROR("Failed to parse include: '{}'", line);
                return {};
            }

            auto include_path = path.parent_path() / include;
            auto include_file = read_file_process_includes(include_path);
            if (!include_file)
                return {};

            s += include_file.value() + '\n';

        } else
            s += line + '\n';
    }

    return s;
}

ShaderSrc read_shader_src(const std::filesystem::path &path) {
    auto str = read_file_process_includes(path);
    fmt::println("#####\n{}#####", str.value_or("Failed to read file"));

    ShaderSrc s{};
    return s;
}

int main() {
    auto s = read_shader_src(HERE / "shader" / "basic.shader");
}
