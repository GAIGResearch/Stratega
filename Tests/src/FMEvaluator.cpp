#include <FMEvaluator.h>

FMEvaluator::FMEvaluator(std::mt19937& rngEngine)
	: rngEngine(&rngEngine)
{
}

std::unique_ptr<FMEvaluationResults> FMEvaluator::evaluate(const SGA::GameConfig& config)
{
	auto results = std::make_unique<FMEvaluationResults>();
	while(results->size() < StepCount)
	{
		if(config.gameType == "TBS")
		{
			auto fm = generateTBSforwardModelFromConfig(config);
			auto state = generateTBSStateFromConfig(config, *rngEngine);
			runGameTBS(*state, fm, *results);
		}
		else
		{
			SGA::RTSForwardModel fm;
			auto state = SGA::generateRTSStateFromConfig(config, *rngEngine);
			//Build Navmesh
			fm.buildNavMesh(*state, SGA::NavigationConfig());
			
			runGameRTS(*state, fm, *results);
		}
	}

	return std::move(results);
}

void FMEvaluator::runGameTBS(SGA::TBSGameState& state, SGA::TBSForwardModel& fm, FMEvaluationResults& results)
{
	while(results.size() < StepCount && !state.isGameOver)
	{
		// Measure getActions speed
		auto getActionsStart = std::chrono::steady_clock::now();
		auto actionSpace = fm.generateActions(state);
		auto getActionsEnd = std::chrono::steady_clock::now();

		// Measure speed of action execution
		std::uniform_int_distribution<int> actionDist(0, actionSpace.size() - 1);
		const auto& action = *(actionDist(*rngEngine) + actionSpace.begin());
		auto executeActionStart = std::chrono::steady_clock::now();
		fm.advanceGameState(state, action);
		auto executeActionEnd = std::chrono::steady_clock::now();

		// Store results
		results.getActionsDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(getActionsEnd - getActionsStart));
		results.executeActionDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(executeActionEnd - executeActionStart));
	}
}

void FMEvaluator::runGameRTS(SGA::RTSGameState& state, SGA::RTSForwardModel& fm, FMEvaluationResults& results)
{
	while (results.size() < StepCount && !state.isGameOver)
	{
		std::uniform_int_distribution<int> playerDist(0, state.players.size());
		
		// Measure getActions speed
		auto getActionsStart = std::chrono::steady_clock::now();
		auto actionSpace = fm.generateActions(state, playerDist(*rngEngine));
		auto getActionsEnd = std::chrono::steady_clock::now();

		// Measure speed of action execution
		std::uniform_int_distribution<int> actionDist(0, actionSpace.size() - 1);
		const auto& action = *(actionDist(*rngEngine) + actionSpace.begin());
		auto executeActionStart = std::chrono::steady_clock::now();
		fm.advanceGameState(state, action);
		auto executeActionEnd = std::chrono::steady_clock::now();

		// Store results
		results.getActionsDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(getActionsEnd - getActionsStart));
		results.executeActionDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(executeActionEnd - executeActionStart));
	}
}