#pragma once

#include "myco/util/helpers.h"
#include "myco/util/log.h"
#include "fmt/format.h"
#include <limits>
#include <ranges>
#include <set>
#include <string>
#include <unordered_map>
#include <string>
#include <vector>

template<typename T>
class PrioListV2 {
    static const std::size_t MAX_SIZE_T;

public:
    PrioListV2() = default;

    void add(const std::string &name, std::set<std::string> deps, T v);

    std::string debug_stringify();

private:
    std::unordered_map<std::string, std::size_t> s_to_i_{};
    std::vector<std::string> i_to_s_{};

    std::vector<std::size_t> o_{};
    std::vector<T> v_{};
    std::vector<std::size_t> idx_{};
    std::vector<std::set<std::size_t>> saved_deps_{};
    std::vector<std::set<std::size_t>> unmet_deps_{};

    void hash_get_i_deps_(
            const std::string &name,
            std::set<std::string> &deps,
            std::size_t &I,
            std::set<std::size_t> &i_deps
    );

    void find_min_and_max_(
            std::size_t I,
            std::set<std::size_t> &deps,
            std::size_t &min, std::size_t &max,
            std::set<std::size_t> &unmet_deps_idxs
    );

    bool bubble_(std::size_t I, std::size_t &idx, std::size_t min_idx);
};

template<typename T>
const std::size_t PrioListV2<T>::MAX_SIZE_T = std::numeric_limits<std::size_t>::max();

template<typename T>
void PrioListV2<T>::add(const std::string &name, std::set<std::string> deps, T v) {
    std::size_t I;
    std::set<std::size_t> I_deps{};
    hash_get_i_deps_(name, deps, I, I_deps);

    if (idx_[I] != MAX_SIZE_T) {
        MYCO_LOG_WARN("{} already added to PrioListV2 at idx {}", name, idx_[I]);
        return;
    }
    saved_deps_[I] = I_deps;

    std::size_t min_idx = 0;
    if (!I_deps.empty()) {
        std::size_t max_idx;
        std::set<std::size_t> unmet_deps_idxs{};
        find_min_and_max_(I, I_deps, min_idx, max_idx, unmet_deps_idxs);

        while (max_idx < min_idx) {
            std::size_t bubble_idx = std::ranges::max(
                    unmet_deps_idxs | std::views::filter([min_idx](auto e) { return e < min_idx; }));
//      I_deps.erase(o_[bubble_idx]);  // We won't need to look for this one anymore

            bool success;
            std::size_t f_idx = bubble_idx;
            do {
                std::size_t old_f_idx = f_idx;
                success = bubble_(I, f_idx, min_idx - 1);
            } while (!success && f_idx < min_idx);

            unmet_deps_idxs.clear();
            find_min_and_max_(I, I_deps, min_idx, max_idx, unmet_deps_idxs);
        }

        if (!unmet_deps_idxs.empty())
            unmet_deps_[I].clear();
    }

    o_.insert(o_.begin() + min_idx, I);
    v_.insert(v_.begin() + min_idx, v);

    idx_[I] = min_idx;
    for (std::size_t i = min_idx + 1; i < o_.size(); ++i)
        if (idx_[o_[i]] != MAX_SIZE_T)
            idx_[o_[i]]++;
}

template<typename T>
void PrioListV2<T>::hash_get_i_deps_(
        const std::string &name,
        std::set<std::string> &deps,
        std::size_t &I,
        std::set<std::size_t> &I_deps
) {
    if (!s_to_i_.contains(name)) {
        s_to_i_[name] = s_to_i_.size();
        i_to_s_.emplace_back(name);
        idx_.emplace_back(MAX_SIZE_T);
        saved_deps_.emplace_back();
        unmet_deps_.emplace_back();
    }
    I = s_to_i_[name];

    for (const auto &d: deps) {
        if (!s_to_i_.contains(d)) {
            s_to_i_[d] = s_to_i_.size();
            i_to_s_.emplace_back(d);
            idx_.emplace_back(MAX_SIZE_T);
            saved_deps_.emplace_back();
            unmet_deps_.emplace_back();
        }
        I_deps.emplace(s_to_i_[d]);
    }
}

template<typename T>
void PrioListV2<T>::find_min_and_max_(
        std::size_t I,
        std::set<std::size_t> &deps,
        std::size_t &min, std::size_t &max,
        std::set<std::size_t> &unmet_deps_idxs
) {
    std::vector<std::size_t> deps_idxs{};
    for (const auto &d: deps) {
        if (idx_[d] != MAX_SIZE_T)
            deps_idxs.emplace_back(idx_[d] + 1);
        else
            unmet_deps_[d].emplace(I);
    }
    min = deps_idxs.empty() ? 0 : std::ranges::max(deps_idxs);

    for (const auto &n: unmet_deps_[I])
        unmet_deps_idxs.emplace(idx_[n]);
    max = unmet_deps_idxs.empty() ? MAX_SIZE_T : std::ranges::min(unmet_deps_idxs);
}

template<typename T>
bool PrioListV2<T>::bubble_(std::size_t I, std::size_t &idx, std::size_t min_idx) {
    while (idx < min_idx) {
        if (saved_deps_[o_[idx + 1]].contains(o_[idx])) {
            idx++;
            return false;
        }

        std::swap(o_[idx], o_[idx + 1]);
        std::swap(v_[idx], v_[idx + 1]);
        idx_[o_[idx]]--;
        idx_[o_[idx + 1]]++;

        idx++;
    }

    return true;
}

template<typename T>
std::string PrioListV2<T>::debug_stringify() {
    std::string s = "[";
    for (const auto [i, e]: myco::enumerate(o_)) {
        if (i != 0)
            s += ", ";
        s += fmt::format("{}", i_to_s_[e]);
    }
    return s + "]";
}
