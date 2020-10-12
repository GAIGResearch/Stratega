#pragma once
#include <vector>
#include <chrono>

struct FMEvaluationResults
{
public:
	std::vector<std::chrono::microseconds> getActionsDurations;
	std::vector<std::chrono::microseconds> executeActionDurations;

	size_t size() const { return getActionsDurations.size(); }

	double computeFPS();
};
