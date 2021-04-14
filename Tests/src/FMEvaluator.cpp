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
		if(config.gameType == SGA::GameType::TBS)
		{
			auto* fm = dynamic_cast<SGA::TBSForwardModel*>(config.forwardModel.get());
			auto state = config.generateGameState();
			runGameTBS(*dynamic_cast<SGA::GameState*>(state.get()), *fm, *results);
		}
		else
		{
			auto* fm = dynamic_cast<SGA::RTSForwardModel*>(config.forwardModel.get());
			auto state = config.generateGameState();
			//Build Navmesh
			fm->buildNavMesh(*dynamic_cast<SGA::GameState*>(state.get()), SGA::NavigationConfig());
			
			runGameRTS(*dynamic_cast<SGA::GameState*>(state.get()), *fm, *results);
		}
	}

	return std::move(results);
}

void FMEvaluator::runGameTBS(SGA::GameState& state, SGA::TBSForwardModel& fm, FMEvaluationResults& results)
{
	while(results.size() < StepCount && !state.isGameOver)
	{
		// Measure getActions speed
		auto getActionsStart = std::chrono::steady_clock::now();
		auto actionSpace = fm.generateActions(state);
		auto getActionsEnd = std::chrono::steady_clock::now();

		// Measure speed of action execution
		std::uniform_int_distribution<size_t> actionDist(0, actionSpace.size() - 1);
		const auto& action = *(actionDist(*rngEngine) + actionSpace.begin());
		auto executeActionStart = std::chrono::steady_clock::now();
		fm.advanceGameState(state, action);
		auto executeActionEnd = std::chrono::steady_clock::now();

		// Store results
		results.getActionsDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(getActionsEnd - getActionsStart));
		results.executeActionDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(executeActionEnd - executeActionStart));
	}
}

void FMEvaluator::runGameRTS(SGA::GameState& state, SGA::RTSForwardModel& fm, FMEvaluationResults& results)
{
	while (results.size() < StepCount && !state.isGameOver)
	{
		std::uniform_int_distribution<size_t> playerDist(0, state.players.size());
		
		// Measure getActions speed
		auto getActionsStart = std::chrono::steady_clock::now();
		auto actionSpace = fm.generateActions(state, static_cast<int>(playerDist(*rngEngine)));
		auto getActionsEnd = std::chrono::steady_clock::now();

		// Measure speed of action execution
		std::uniform_int_distribution<size_t> actionDist(0, actionSpace.size() - 1);
		SGA::ActionAssignment actions;
		actions.assignActionOrReplace(actionSpace[actionDist(*rngEngine)]);
		auto executeActionStart = std::chrono::steady_clock::now();
		fm.advanceGameState(state, actions);
		auto executeActionEnd = std::chrono::steady_clock::now();

		// Store results
		results.getActionsDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(getActionsEnd - getActionsStart));
		results.executeActionDurations.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(executeActionEnd - executeActionStart));
	}
}