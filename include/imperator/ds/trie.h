#ifndef IMPERATOR_DS_TRIE_H
#define IMPERATOR_DS_TRIE_H

#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace imp {
template<typename T = void>
class Trie {
public:
  Trie();

  void insert(const std::string &s, std::optional<T> user_data);

  bool contains(const std::string &s);

  /* Search the tree for words that start with a given prefix.
   * Set n to anything except 0 and at most n words will be returned.
   *
   * This will get all words that actually start with our prefix (assuming they exist) first,
   * then traverse the rest of the tree in alphabetical order.
   *
   * Words which have the same prefix are given a "distance" of 0 so they show up first, then
   * all other words retrieved have their levenshtein distance calculated for real--this means
   * words with similar length to the currently typed prefix will be favored over longer words,
   * and in general the length of returned words will increase down the list.
   */
  std::vector<std::pair<std::string, std::optional<T>>>
  fuzzy_match_n(const std::string &pre, std::size_t n = 0) const;

private:
  struct TrieNode {
    std::map<char, std::unique_ptr<TrieNode>, std::greater<>> children{};
    std::optional<std::string> value{std::nullopt};
    std::optional<T> user_data{std::nullopt};
  };

  std::unique_ptr<TrieNode> root{};

  struct FuzzyData {
    std::size_t distance;
    std::string s;
    std::optional<T> user_data;

    auto operator<=>(const FuzzyData &other) const;
  };

  static std::size_t levenshtein_distance(const std::string &s1, const std::string &s2);
};

template<typename T>
Trie<T>::Trie() {
  root = std::make_unique<TrieNode>();
}

template<typename T>
void Trie<T>::insert(const std::string &s, std::optional<T> user_data) {
  TrieNode *curr = root.get();
  for (const auto &c: s) {
    if (auto it = curr->children.find(c); it == curr->children.end()) {
      auto tmp = std::make_unique<TrieNode>();
      TrieNode *old_curr = curr;
      curr = tmp.get();
      old_curr->children[c] = std::move(tmp);
    } else {
      curr = it->second.get();
    }
  }
  curr->value = s;
  curr->user_data = user_data;
}

template<typename T>
bool Trie<T>::contains(const std::string &s) {
  TrieNode *curr = root.get();
  for (const auto &c: s) {
    if (auto it = curr->children.find(c); it == curr->children.end()) {
      return false;
    } else {
      curr = it->second.get();
    }
  }
  return curr->value != std::nullopt;
}

template<typename T>
std::vector<std::pair<std::string, std::optional<T>>>
Trie<T>::fuzzy_match_n(const std::string &pre, std::size_t n) const {
  std::vector<std::pair<std::string, std::optional<T>>> matches{};
  if (pre.empty()) {
    return matches;
  }

  std::stack<TrieNode *> s{};
  std::priority_queue<FuzzyData, std::vector<FuzzyData>, std::greater<>> pq{};

  // Iterate through the prefix until we either can't find the next character or run out
  TrieNode *curr = root.get();
  for (const auto &c: pre) {
    s.push(curr); // We will save all the intermediate steps on the stack
    if (auto it = curr->children.find(c); it == curr->children.end())
      break;
    else
      curr = it->second.get();
  }
  s.push(curr);

  // DFS from our last position
  std::set<TrieNode *> visited{};
  while (!s.empty()) {
    curr = s.top();
    s.pop();

    const bool is_visited = visited.contains(curr);
    visited.emplace(curr);

    if (!is_visited && curr->value) {
      auto v = *curr->value;
      auto distance = v.starts_with(pre) ? 0 : levenshtein_distance(pre, *curr->value);
      pq.emplace(distance, v, curr->user_data);
      if (n != 0 && matches.size() == n) {
        break;
      }
    }

    if (!is_visited && !curr->children.empty()) {
      s.push(curr);
      for (const auto &c: curr->children) {
        s.push(c.second.get());
      }
    }
  }

  while (!pq.empty()) {
    matches.emplace_back(pq.top().s, pq.top().user_data);
    pq.pop();
  }

  return matches;
}

template<typename T>
auto Trie<T>::FuzzyData::operator<=>(const FuzzyData &other) const {
  return distance <=> other.distance;
}

template<typename T>
std::size_t Trie<T>::levenshtein_distance(const std::string &s1, const std::string &s2) {
  std::vector<std::size_t> v0(s2.size() + 1, 0);
  std::vector<std::size_t> v1(s2.size() + 1, 0);

  for (std::size_t i = 0; i <= s2.size(); ++i) {
    v0[i] = i;
  }

  for (std::size_t i = 0; i <= s1.size() - 1; ++i) {
    v1[0] = i + 1;
    for (std::size_t j = 0; j <= s2.size() - 1; ++j) {
      auto deletion_cost = v0[j + 1] + 1;
      auto insertion_cost = v1[j] + 1;
      auto substitution_cost = s1[i] == s2[j] ? v0[j] : v0[j] + 1;

      v1[j + 1] = std::min({deletion_cost, insertion_cost, substitution_cost});
    }
    std::swap(v0, v1);
  }

  return v0[s2.size()];
}
} // namespace imp

#endif//IMPERATOR_DS_TRIE_H
