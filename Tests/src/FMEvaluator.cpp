#include <FMEvaluator.h>

FMEvaluator::FMEvaluator(std::mt19937& rngEngine)
	: rngEngine(&rngEngine)
{
}

std::unique_ptr<FMEvaluationResults> FMEvaluator::evaluate(const SGA::GameConfig& config, SGA::TBSForwardModel& fm)
{
	auto results = std::make_unique<FMEvaluationResults>();
	while(results->size() < StepCount)
	{
		runGame(*SGA::generateTBSStateFromConfig(config, *rngEngine), fm, *results);
	}

	return std::move(results);
}

void FMEvaluator::runGame(SGA::TBSGameState& state, SGA::TBSForwardModel& fm, FMEvaluationResults& results)
{
	while(results.size() < StepCount && !state.isGameOver)
	{
		// Measure getActions speed
		auto getActionsStart = std::chrono::steady_clock::now();
		auto actionSpace = fm.getActions(state);
		auto getActionsEnd = std::chrono::steady_clock::now();

		// Measure speed of action execution
		std::uniform_int_distribution<int> actionDist(0, actionSpace->count() - 1);
		const auto& action = *(actionDist(*rngEngine) + actionSpace->begin());
		auto executeActionStart = std::chrono::steady_clock::now();
		fm.advanceGameState(state, action);
		auto executeActionEnd = std::chrono::steady_clock::now();

		// Store results
		results.getActionsDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(getActionsEnd - getActionsStart));
		results.executeActionDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(executeActionEnd - executeActionStart));
	}
}