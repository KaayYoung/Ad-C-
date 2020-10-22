#include "comp6771/word_ladder.hpp"
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <queue>
#include <string>
#include <vector>

namespace word_ladder {
	namespace views = ranges::views;
	auto extract_same_length(std::string const& from, absl::flat_hash_set<std::string> const& lexicon)
	   -> absl::flat_hash_set<std::string> {
		auto same_length = [from](auto& lexicon) { return lexicon.size() == from.length(); };
		return lexicon | views::filter(same_length) | ranges::to<absl::flat_hash_set<std::string>>;
	}

	// Use dfs to find the shorest path
	auto dfs(std::string const& curr,
	         std::string const& des,
	         absl::flat_hash_map<std::string, std::vector<std::string>> const& neighbors,
	         absl::flat_hash_map<std::string, int> const& depth,
	         std::vector<std::string>& path,
	         std::vector<std::vector<std::string>>& res) -> void {
		path.push_back(curr);
		// If the curr is the des, add this path into res and return
		if (curr == des) {
			res.push_back(path);
			return;
		}

		// Need to check whether curr is in neighbours
		if (neighbors.find(curr) != neighbors.end()) {
			for (auto const& n : neighbors.at(curr)) {
				// If the depth of the neighbour is curr + 1, that means they are adjacent
				if (depth.at(n) == depth.at(curr) + 1) {
					dfs(n, des, neighbors, depth, path, res);
					path.pop_back(); // Drop last node due to dfs
				}
			}
		}
	}

	auto generate(std::string const& from,
	              std::string const& to,
	              absl::flat_hash_set<std::string> const& lexicon)
	   -> std::vector<std::vector<std::string>> {
	    // extrace words which has same length with the from and to
		absl::flat_hash_set<std::string> copy_lexicon = extract_same_length(from, lexicon);
		auto all_paths = std::vector<std::vector<std::string>>{};
		auto visited = absl::flat_hash_set<std::string>{};
		auto neighbors = absl::flat_hash_map<std::string, std::vector<std::string>>{};
		auto depth = absl::flat_hash_map<std::string, int>{};
		auto path_q = std::queue<std::string>();

		path_q.push(from);
		depth[from] = 0;

		int d = 1; // depth of nodes

		while (!path_q.empty()) {
			unsigned long size_q = path_q.size();

			for (unsigned long i = 0; i < size_q; ++i) {
				std::string curr = path_q.front();
				path_q.pop();
				auto new_curr = curr;
				for (char& ch : new_curr) // Each time replace one character
				{
					auto old_ch = ch;
					for (char c = 'a'; c <= 'z'; ++c) {
						// Keep a copy of the original character
						ch = c;
						// If can find the revised word, that means curr can reach to this word
						if (copy_lexicon.find(new_curr) != copy_lexicon.end()) {
							neighbors[curr].push_back(new_curr);
							visited.emplace(new_curr);
							if (depth.find(new_curr) == depth.end()) {
								depth[new_curr] = d;
								path_q.push(new_curr);
							}
						}
					}
					ch = old_ch; // Roll back the revised character
				}
			}
			++d; // Depth + 1 when for loop end
			for (auto& nodes : visited) {
				copy_lexicon.erase(nodes);
			}
			visited.clear();
		}

		auto single_path = std::vector<std::string>();
		// Use dfs to find the shortes path
		dfs(from, to, neighbors, depth, single_path, all_paths);

		// sort paths
		ranges::sort(all_paths, std::less<>());
		return all_paths;
	}

} // namespace word_ladder
