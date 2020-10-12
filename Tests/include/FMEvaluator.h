#pragma once
#include <random>
#include <FMEvaluationResults.h>
#include <ForwardModel/TBSForwardModel.h>
#include <Configuration/GameConfig.h>

typedef std::unique_ptr<SGA::TBSGameState> (*StateGenerator)(std::mt19937& rngEngine);

class FMEvaluator
{
public:
	FMEvaluator(std::mt19937& rngEngine);
	
	size_t StepCount = 100000;
	std::unique_ptr<FMEvaluationResults> evaluate(const SGA::GameConfig& config, SGA::TBSForwardModel& fm);

private:
	void runGame(SGA::TBSGameState& state, SGA::TBSForwardModel& fm, FMEvaluationResults& results);
	
	std::mt19937* rngEngine;
};
