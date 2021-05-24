#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>
#include <Stratega/Agent/StateAbstraction/AbstractMCTSNode.h>
#include <Stratega/Agent/StateAbstraction/AbstractMCTSParameters.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>

namespace SGA
{
	class AbstractStateMCTSAgent : public Agent
	{
	private:
		std::unique_ptr<AbstractMCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		AbstractMCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	
	public:

		AbstractStateMCTSAgent(AbstractMCTSParameters&& params) :
			Agent{}, parameters_(std::move(params))
		{
			//Evaluated fitness: 20 and Wins: 6 at K=100, RL=5, OS=1, 

			//K=100, RL=5, OS=1, 
			parameters_.K = 100;
			parameters_.ROLLOUT_LENGTH = 5;
			//parameters_.OPPONENT_MODEL = std::make_unique<AttackWeakestOpponentScript>(); // the opponent model has not been correctly set in the NTBEA evaluation

			// Map=1, Positions=1, AttackDamage=1, HealRange=1, AttackRange=1, Health=1, MovementPoints=1, HealAmount=1,
			params.STATE_FACTORY = nullptr;
			StateFactoryConfiguration configuration;
			configuration.insertMap = true;
			configuration.insertEntityPositions = true;
			configuration.insertEntityParameters["AttackDamage"] = true;
			configuration.insertEntityParameters["HealRange"] = true;
			configuration.insertEntityParameters["AttackRange"] = true;
			configuration.insertEntityParameters["Health"] = true;
			configuration.insertEntityParameters["MovementPoints"] = true;
			configuration.insertEntityParameters["HealAmount"] = true;
			
			parameters_.STATE_FACTORY = std::make_unique<StateFactory>(configuration);
		}
		
		ActionAssignment computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs) override;

	};
}
