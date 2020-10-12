#include <FMEvaluationResults.h>

double FMEvaluationResults::computeFPS()
{
	double timeSumSeconds = 0;
	for(auto i = 0; i < size(); i++)
	{
		timeSumSeconds += (executeActionDurations[i] + getActionsDurations[i]).count() / 1000000.0;
	}

	return size() / timeSumSeconds;
}
