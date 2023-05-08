#include "pl_v1.h"
#include "pl_v2.h"
#include "pl_v3.h"
#include "pl_v4.h"
#include "pl_v5.h"
#include "pl_v6.h"
#include "pl_v7.h"
#include "myco/util/helpers.h"
#include "myco/util/rnd.h"
#include "myco/util/time.h"
#include "fmt/format.h"
#include "rapidcsv.h"
#include <algorithm>
#include <filesystem>

auto HERE = std::filesystem::path(__FILE__).parent_path();

template<template<typename> typename T, typename IT, typename DT>
double benchmark(IT &items, DT &deps, const std::string &check_string) {
    auto sw = myco::Stopwatch();
    auto pl = T<int>();
    for (const auto &[i, e]: myco::enumerate(items))
        pl.add(e, deps[e], i);
    if (pl.debug_stringify() != check_string)
        MYCO_LOG_ERROR("Bad {} {}", pl.debug_stringify(), check_string);
    sw.stop();

    return sw.elapsed_sec();
}

int main(int, char *[]) {
    auto d = rapidcsv::Document("", rapidcsv::LabelParams(-1, -1));
    d.SetRow<std::string>(0, {"N", "V2", "V3", "V4", "V5", "V6"});

    for (auto N = 500, row = 1; N <= 5'000; N += 500, row++) {
        d.SetCell(0, row, N);

        std::vector<std::string> items{};
        std::unordered_map<std::string, std::set<std::string>> deps_s{};
        std::unordered_map<std::string, std::vector<std::string>> deps_v{};

        for (auto i = 0; i < N; ++i) {
            std::string s(10, 0);
            std::generate_n(s.begin(), 10, []() { return myco::get<char>(65, 90); });
            items.emplace_back(s);
        }
        std::ranges::sort(items);

        std::string check_string = "[";
        for (const auto &[i, e]: myco::enumerate(items))
            check_string += fmt::format("{}{}", i != 0 ? ", " : "", e);
        check_string += "]";

        deps_s[items[1]] = {items[0]};
        deps_v[items[1]] = {items[0]};
        deps_s[items[2]] = {items[1], items[0]};
        deps_v[items[2]] = {items[1], items[0]};
        for (std::size_t i = 2; i < items.size() - 1; i += 1) {
            deps_s[items[i + 1]] = {items[i], items[i - 1], items[i - 2]};
            deps_v[items[i + 1]] = {items[i], items[i - 1], items[i - 2]};
        }

        myco::shuffle(items);
        std::size_t col = 1;
//        d.SetCell(col++, row, benchmark<PrioListV1>(items, deps_s, check_string));
        d.SetCell(col++, row, benchmark<PrioListV2>(items, deps_s, check_string));
        d.SetCell(col++, row, benchmark<PrioListV3>(items, deps_s, check_string));
        d.SetCell(col++, row, benchmark<PrioListV4>(items, deps_v, check_string));
        d.SetCell(col++, row, benchmark<PrioListV5>(items, deps_v, check_string));
        d.SetCell(col++, row, benchmark<PrioListV6>(items, deps_v, check_string));
//        d.SetCell(col++, row, benchmark<PrioListV7>(items, deps_s, check_string));

        fmt::println("{}", d.GetRow<double>(row));
    }

    d.Save((HERE / "out.csv").string());

    std::filesystem::current_path(HERE);
    std::system("python graph_csv.py \"PrioList N vs Insert Time\" 5");
}
