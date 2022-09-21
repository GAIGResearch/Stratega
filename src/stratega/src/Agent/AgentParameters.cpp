#include <Stratega/Agent/AgentParameters.h>
#include <Stratega/Agent/ActionScripts/SkipTurnScript.h>

namespace SGA {
	

	AgentParameters::AgentParameters()
	{
		opponentModel = std::make_shared<SkipTurnScript>();
		budgetType = Budget::UNDEFINED;
		portfolio = std::vector<std::shared_ptr<BaseActionScript>>();
	}

	void AgentParameters::resetCounters(const Timer& timerToReset)
	{
		this->timer = timerToReset;
		currentFMCalls = 0;
		currentIterations = 0;
	}

	bool AgentParameters::isBudgetOver() const
	{
		switch (budgetType)
		{
		case Budget::TIME:
			return timer.percCompletedTime() > percTimeBudget;
		case Budget::ITERATIONS:
			return currentIterations >= maxIterations;
		case Budget::FMCALLS:
			return currentFMCalls >= maxFMCalls;
		case Budget::UNDEFINED:
			return false;
		}
		return false;

	}

	std::unique_ptr<ForwardModel> AgentParameters::preprocessForwardModel(const ForwardModel& forwardModel)
	{
		bool isPortfolio = !portfolio.empty();

		if (forwardModel.getGameType() == SGA::GameType::TBS)
			if (isPortfolio)	return std::make_unique<PortfolioTBSForwardModel>(dynamic_cast<const TBSForwardModel&>(forwardModel), portfolio);
			else				return std::make_unique<TBSForwardModel>(dynamic_cast<const TBSForwardModel&>(forwardModel));

		else if (forwardModel.getGameType() == SGA::GameType::RTS)
			if (isPortfolio)	return std::make_unique<PortfolioRTSForwardModel>(dynamic_cast<const RTSForwardModel&>(forwardModel), portfolio);
			else				return std::make_unique<RTSForwardModel>(dynamic_cast<const RTSForwardModel&>(forwardModel));

		else throw std::runtime_error("Unrecognized forward model type in Agent Parameters.");
	}

	void AgentParameters::decode(const YAML::Node& node)
	{
		maxFMCalls             = node["MaxFmCalls"].as<int>(maxFMCalls);
		maxIterations          = node["Iterations"].as<int>(maxIterations);
		percTimeBudget         = node["PercTime"].as<double>(percTimeBudget);
        HEURISTIC        = node["Heuristic"].as<std::string>(HEURISTIC);

		if (node["Budget"].as<std::string>("") == "TIME")
			budgetType = SGA::Budget::TIME;
		else if (node["Budget"].as<std::string>("") == "FMCALLS")
			budgetType = SGA::Budget::FMCALLS;
		else if (node["Budget"].as<std::string>("") == "ITERATIONS")
			budgetType = SGA::Budget::ITERATIONS;
	}


	void AgentParameters::printDetails() const
	{
		std::cout << "Agent with parameters:" << "\n";
		std::cout << "\tPLAYER_ID: " << PLAYER_ID << "\n";
		std::cout << "\tBudget type: " << budgetTypeStr(budgetType) << std::endl;
		std::cout << "\tMax FM Calls (" << (budgetType == Budget::FMCALLS ? "active" : "inactive") << "): " << maxFMCalls << std::endl;
		std::cout << "\tMax Iterations (" << (budgetType == Budget::ITERATIONS ? "active" : "inactive") << "): " << maxIterations << std::endl;
		std::cout << "\tStop at Perc Time (" << (budgetType == Budget::TIME ? "active" : "inactive") << "): " << percTimeBudget << std::endl;
		std::cout << "\tCurrent FM Calls: " << currentFMCalls << std::endl;
		std::cout << "\tCurrent iterations: " << currentIterations << std::endl;
		std::cout << "\tScripts in portfolio: " << portfolio.size() << std::endl;
		std::cout << "\tOpponent model by script: " << opponentModel->toString() << std::endl;
		std::cout << "\tState evaluation heuristic: " << heuristic->getName() << std::endl;
	}
}