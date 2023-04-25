#include "myco/util/sops.h"
#include "myco/util/log.h"
#include "myco/util/time.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "re2/re2.h"
#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
#include <string>
#include <sstream>

auto HERE = std::filesystem::path(__FILE__).parent_path();

struct ShaderSrc {
    std::optional<std::string> name{};
    std::optional<std::string> vertex{};
    std::optional<std::string> fragment{};
};

std::optional<std::string> read_file_process_includes(
        const std::filesystem::path &path,
        std::vector<std::filesystem::path> &included_paths
) {
    const static RE2 spaces_pat{R"((\s+))"};
    assert(spaces_pat.ok());

    const static RE2 path_pat{R"("(.+)\")"};
    assert(path_pat.ok());

    included_paths.emplace_back(path);

    std::ifstream ifs(path.string());
    if (!ifs.is_open()) {
        MYCO_LOG_ERROR("Failed to open file: '{}'", path.string());
        return std::nullopt;
    }

    std::string s;
    std::size_t line_no = 1;
    for (std::string line; std::getline(ifs, line); ++line_no) {
        myco::rtrim(line);

        if (line.starts_with("#include")) {
            auto tokens = myco::split(line, spaces_pat);

            std::string include;
            if (tokens.size() < 2 || !RE2::FullMatch(tokens[1], path_pat, &include)) {
                MYCO_LOG_ERROR("Failed to parse include in file {}:{}: '{}'", path.string(), line_no, line);
                return std::nullopt;
            }

            auto include_path = path.parent_path() / include;
            if (std::ranges::contains(included_paths, include_path))
                continue;  // Don't try and include a file twice

            auto include_file = read_file_process_includes(include_path, included_paths);
            if (!include_file)
                return std::nullopt;

            s += *include_file + '\n';

        } else
            s += line + '\n';
    }

    return s;
}

std::optional<ShaderSrc> try_parse_shader_src(const std::string &src) {
    enum class BufferDst {vertex, fragment};

    // Match things of the form
    //   #pragma <name>(<args>)<bad>\n
    // Captures what is after the parens to emit warnings
    const static RE2 pragma_w_args(R"(#pragma (.+)\((.+)\)(.*))");
    assert(pragma_w_args.ok());

    // Match things of the form
    //   #pragma <name>\n
    // Should reject parens
    const static RE2 pragma_no_args(R"(#pragma ([^()\n]+))");
    assert(pragma_no_args.ok());

    ShaderSrc s{};
    // Intermediate storage control
    std::string buffer;
    bool read_non_pragma_line = false;
    BufferDst buffer_dst{BufferDst::vertex};

    auto try_set_shader_part = [&](){
        if (!read_non_pragma_line) {
            buffer.clear();
            return;
        }

        switch (buffer_dst) {
            using enum BufferDst;
            case vertex:
                if (!s.vertex)
                    s.vertex = buffer;
                break;

            case fragment:
                if (!s.fragment)
                    s.fragment = buffer;
                break;
        }

        buffer.clear();
    };

    // Variables to store regex match captures
    std::string pragma_name;
    std::string pragma_args;
    std::string pragma_extra;

    std::istringstream iss{src};
    std::size_t line_no = 1;
    for (std::string line; std::getline(iss, line); ++line_no) {
        myco::rtrim(line);

        if (line.empty()) {
            if (!buffer.empty())
                buffer.append("\n");
            continue;
        }

        if (RE2::FullMatch(line, pragma_w_args, &pragma_name, &pragma_args, &pragma_extra)) {
            if (pragma_name == "name")
                s.name = pragma_args;
            else
                SPDLOG_WARN("Unrecognized pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

            if (!pragma_extra.empty())
                SPDLOG_WARN("Trailing characters on pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

        } else if (RE2::FullMatch(line, pragma_no_args, &pragma_name)) {
            if (pragma_name == "vertex") {
                if (s.vertex)
                    SPDLOG_WARN("Duplicate vertex pragma in shader {}:{}, will ignore", s.name.value_or("undef"), line_no);
                try_set_shader_part();
                buffer_dst = BufferDst::vertex;

            } else if (pragma_name == "fragment") {
                if (s.fragment)
                    SPDLOG_WARN("Duplicate fragment pragma in shader {}:{}, will ignore", s.name.value_or("undef"), line_no);
                try_set_shader_part();
                buffer_dst = BufferDst::fragment;

            } else
                SPDLOG_WARN("Unrecognized pragma in shader {}:{}: {}", s.name.value_or("undef"), line_no, line);

        } else {
            read_non_pragma_line = true;
            buffer.append(line + '\n');
        }
    }

    if (!buffer.empty())
        try_set_shader_part();

    return s;
}

std::optional<ShaderSrc> parse_shader_src(const std::filesystem::path &path) {
    std::vector<std::filesystem::path> included_paths{};
    auto src = read_file_process_includes(path, included_paths);
    if (!src)
        return std::nullopt;

    return try_parse_shader_src(*src);
}

std::optional<ShaderSrc> parse_shader_src(const std::string &src) {
    return try_parse_shader_src(src);
}

int main() {
    MYCO_LOG_INFO("{}", myco::timestamp());

    auto s = parse_shader_src(HERE / "shader" / "basic.shader");
    fmt::println("Name: {}", *s->name);
    fmt::println("Vertex shader:\n#####\n{}#####", *s->vertex);
    fmt::println("Fragment shader:\n#####\n{}#####", *s->fragment);
}
