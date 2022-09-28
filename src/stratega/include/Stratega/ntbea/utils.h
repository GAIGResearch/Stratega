#pragma once
#include <functional>
#include <unordered_set>
#include <vector>

typedef std::function<void(const std::vector<int>& combination)> CallbackFn;
void generateCombinationsImpl(int choices, std::vector<int>& comb, size_t index, std::unordered_set<int>& used, CallbackFn callback)
{
	if (index == comb.size())
	{
		callback(comb);
		return;
	}

	for (int i = 0; i < choices; i++)
	{
		if (used.find(i) != used.end())
			continue;
		
		comb[index] = i;
		used.insert(i);
		generateCombinationsImpl(choices, comb, index + 1, used, callback);
		used.erase(used.find(i));
	}
}

void generateCombinations(int choices, int k, CallbackFn callback)
{
	std::vector<int> combinationBucket(k);
	std::unordered_set<int> used;
	generateCombinationsImpl(choices, combinationBucket, 0, used, callback);
}