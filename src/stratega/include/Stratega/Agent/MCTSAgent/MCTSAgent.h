#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/MCTSAgent/MCTSNode.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSResourceHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSTechnologyHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>

#include "MCTSParameters.h"

namespace SGA
{

	class MCTSAgent : public Agent
	{
	public:
		explicit MCTSAgent(const std::string& name, MCTSParameters&& params)
			:Agent{ name }, parameters_(std::move(params))
		{
		}

		/// <summary>
		/// Override from Agent. This function is called at the start of the game only, providing a copy of the initial game state,
		/// a foward model, and a timer that indicates when the game will start.
		/// </summary>
		void init(GameState initialState, const ForwardModel& forwardModel, Timer timer) override;

		/// <summary>
		/// Override from agents. This function is called at every tick of the game (turn in TBS, frame in RTS). The agent must return an
		/// action assignment object that indicates what is to be executed in the game. This function receives a copy of the game state, a 
		/// forward model and a timer that indicates when the action is due.  In TBS games, the game is
		/// blocked waiting for actions for eac agent, and penalties may apply if this time is not respected. In RTS games, 
		/// the game will continue independently of having returned an action, when the timer goes off.
		/// </summary>
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;

	private:
		std::unique_ptr<MCTSNode> rootNode = nullptr;		//Root node of MCTS.
		int previousActionIndex = -1;						//Indicates the index of the action executed in the previous compute call.
		MCTSParameters parameters_;							//Configuration of MCTS.
        //std::shared_ptr<BasicTBSResourceHeuristic> debug_heuristic = nullptr;
        std::shared_ptr<BasicTBSTechnologyHeuristic> debug_heuristic = nullptr;
        //std::shared_ptr<BasicTBSCombatHeuristic> debug_heuristic = nullptr;

        // variables for abstraction
        std::map< int, std::vector< MCTSNode* > > depthToNodes;

        // first dimension: depth, second dimension: absnode Index, second
        std::vector< std::vector< std::vector< MCTSNode* > > > absNodes;

        /*1st, 2nd, and 3rd dimension of absNodes as key, pair of <value,
        * visitCount> as value
        */
        std::map< int, std::vector< double > > absNodeToStatistics;

        int global_absNodeIndex = 0;
        std::map< int, int > treeNodetoAbsNode;

        // functions for abstraction
        void stepInit();

        void deleteAbstraction()
        {
            // initialize the abstraction-related data as empty
            absNodes = std::vector< std::vector< std::vector< MCTSNode* > > >();
            for(int i = 0; i < 100; i++) {
                absNodes.push_back(std::vector< std::vector< MCTSNode* > >());
            }

            absNodeToStatistics = std::map< int, std::vector< double > >();
            treeNodetoAbsNode = std::map< int, int >();
        }

        bool isTwoNodeApproxmateHomomorphism(
            const ForwardModel& forwardModel,
            MCTSNode* node1,
            MCTSNode* node2,
            double reward_threshold,
            double transition_threshold);
    };

}