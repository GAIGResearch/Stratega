#include <FMEvaluationResults.h>

double FMEvaluationResults::computeFPS()
{
	auto sum = executeActionDurations[0] + getActionsDurations[0];
	for(auto i = 1; i < size(); i++)
	{
		sum += executeActionDurations[i] + getActionsDurations[i];
	}

	auto timeSumSeconds = std::chrono::duration<double>(sum).count();
	return size() / timeSumSeconds;
}
