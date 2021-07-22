#pragma once
#include <cassert>
#include <functional>
#include <unordered_set>
#include <vector>

typedef std::function<void(const std::vector<int>& combination)> CallbackFn;
inline void generateCombinationsImpl(size_t choices, std::vector<int>& comb, size_t index, std::unordered_set<int>& used, CallbackFn callback)
{
	if (index == comb.size())
	{
		callback(comb);
		return;
	}

	for (int i = 0; i < static_cast<int>(choices); i++)
	{
		if (used.find(i) != used.end())
			continue;
		
		comb[index] = i;
		used.insert(i);
		generateCombinationsImpl(choices, comb, index + 1, used, callback);
		used.erase(used.find(i));
	}
}

inline void generateCombinations(size_t choices, size_t k, CallbackFn callback)
{
	assert(k <= choices);
	
	std::vector<int> combinationBucket(k);
	std::unordered_set<int> used;
	generateCombinationsImpl(choices, combinationBucket, 0, used, callback);
}