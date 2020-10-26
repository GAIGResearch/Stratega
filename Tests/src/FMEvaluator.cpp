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
			auto fm = forwardModelFromConfig(config);
			auto state = generateTBSStateFromConfig(config, *rngEngine);
			runGameTBS(*state, fm, *results);
		}
		else
		{
			SGA::RTSForwardModel fm;
			auto state = SGA::generateRTSStateFromConfig(config, *rngEngine);
			//Build Navmesh
			fm.buildNavMesh(*state);
			
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

void FMEvaluator::runGameRTS(SGA::RTSGameState& state, SGA::RTSForwardModel& fm, FMEvaluationResults& results)
{
	while (results.size() < StepCount && !state.isGameOver)
	{
		// Generate actions for every player
		std::vector<SGA::Action<SGA::Vector2f>> actions;
		std::chrono::duration<long long, std::ratio<1, 1000000>> actionSum{};
		for(const auto& player : state.players)
		{
			// Measure getActions speed
			auto getActionsStart = std::chrono::steady_clock::now();
			auto actionSpace = fm.getActions(state, player.playerID);
			auto getActionsEnd = std::chrono::steady_clock::now();
			actionSum += std::chrono::duration_cast<std::chrono::microseconds>(getActionsEnd - getActionsStart);
			
			std::uniform_int_distribution<int> actionDist(0, actionSpace->count() - 1);
			for (int i = 0; i < 10; i++)
			{
				actions.emplace_back(actionSpace->getAction(actionDist(*rngEngine)));
			}
		}

		// Measure speed of action execution
		auto executeActionStart = std::chrono::steady_clock::now();
		fm.advanceGameState(state, actions);
		auto executeActionEnd = std::chrono::steady_clock::now();

		// Store results
		results.getActionsDurations.emplace_back(actionSum / state.players.size()); // We just want the time it takes to generate actions for one player
		results.executeActionDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(executeActionEnd - executeActionStart));
	}
}