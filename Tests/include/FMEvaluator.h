#pragma once
#include <random>
#include <FMEvaluationResults.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/Configuration/GameConfig.h>

typedef std::unique_ptr<SGA::GameState> (*StateGenerator)(std::mt19937& rngEngine);

class FMEvaluator
{
public:
	FMEvaluator(std::mt19937& rngEngine);
	
	size_t StepCount = 100000;
	std::unique_ptr<FMEvaluationResults> evaluate(const SGA::GameConfig& config);

private:
	void runGameTBS(SGA::GameState& state, SGA::TBSForwardModel& fm, FMEvaluationResults& results);
	void runGameRTS(SGA::GameState& state, SGA::RTSForwardModel& fm, FMEvaluationResults& results);
	
	std::mt19937* rngEngine;
};
