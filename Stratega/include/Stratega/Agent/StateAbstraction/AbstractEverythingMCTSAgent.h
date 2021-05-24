#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>
#include <Stratega/Agent/StateAbstraction/AbstractMCTSNode.h>
#include <Stratega/Agent/StateAbstraction/AbstractMCTSParameters.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>


namespace SGA
{
	class AbstractEverythingMCTSAgent : public Agent
	{
	private:
		std::unique_ptr<AbstractMCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		AbstractMCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	
	public:

		AbstractEverythingMCTSAgent(AbstractMCTSParameters&& params) :
			Agent{}, parameters_(std::move(params))
		{
			//K = 100, RL = 1, OS = 2,
			parameters_.K = 100;
			parameters_.ROLLOUT_LENGTH = 1;
			//parameters_.OPPONENT_MODEL = std::make_unique<RandomActionScript>();	// the opponent model has not been correctly set in the NTBEA evaluation
			
			//  Scripts = { AC AW RF 
			parameters_.PORTFOLIO = std::vector<std::unique_ptr<BaseActionScript>>();
			std::unique_ptr<BaseActionScript> attackClose = std::make_unique<AttackClosestOpponentScript>();
			parameters_.PORTFOLIO.emplace_back(std::move(attackClose));
			std::unique_ptr<BaseActionScript> attackWeak = std::make_unique<AttackWeakestOpponentScript>();
			parameters_.PORTFOLIO.emplace_back(std::move(attackWeak));
			std::unique_ptr<BaseActionScript> run = std::make_unique<RunToFriendlyUnitsScript>();
			parameters_.PORTFOLIO.emplace_back(std::move(run));

			
			// Map = 0, Positions = 0, AttackDamage = 1, HealRange = 1, AttackRange = 0, Health = 0, MovementPoints = 1, HealAmount = 0,
			params.STATE_FACTORY = nullptr;
			StateFactoryConfiguration configuration;
			configuration.insertMap = false;
			configuration.insertEntityPositions = false;
			configuration.insertEntityParameters["AttackDamage"] = true;
			configuration.insertEntityParameters["HealRange"] = true;
			configuration.insertEntityParameters["AttackRange"] = false;
			configuration.insertEntityParameters["Health"] = false;
			configuration.insertEntityParameters["MovementPoints"] = true;
			configuration.insertEntityParameters["HealAmount"] = false;

			parameters_.STATE_FACTORY = std::make_unique<StateFactory>(configuration);

		}
		
		ActionAssignment computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs) override;

	};
}
