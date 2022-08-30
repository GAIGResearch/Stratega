#pragma once
#include <Stratega/Agent/MCTSAgent/MCTSParameters.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>

namespace SGA {

	class MCTSNode : public ITreeNode<MCTSNode>
	{
	public:
		int nodeDepth = 0;			//Depth of this node in the tree.
		int treesize = 1;			//Reference to the number of nodes in the tree behind this node
        int childExpanded = 0;

	protected:
		int nVisits = 0;			//number of visits to this node.
		double bounds[2] = { 0, 1 };	//Reward bounds in this node.


	public:

		/// <summary>
		/// Initializes the node.
		/// </summary>
		void initializeNode();

		/// <summary>
		/// MCTS Tree Policy phase.
		/// </summary>
		MCTSNode* treePolicy(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator);

		/// <summary>
		/// MCTS Expansion phase.
		/// </summary>
		MCTSNode* expand(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator);

		/// <summary>
		/// Chooses a child node with the highest UCB value.
		/// </summary>
		MCTSNode* uct(MCTSParameters& params, boost::mt19937& randomGenerator);

		/// <summary>
		/// MCTS Rollout phase.
		/// </summary>
		double rollOut(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator);

		/// <summary>
		/// Indicates if a rollout should be completed.
		/// </summary>
		static bool rolloutFinished(GameState& rollerState, int depth, MCTSParameters& params);

		/// <summary>
		/// MCTS Back-propagation phase.
		/// </summary>
		static void backUp(MCTSNode* node, double result);

		/// <summary>
		/// Returns the best action found by the tree.
		/// </summary>
		int bestAction(MCTSParameters& params, boost::mt19937& randomGenerator);

		// helper function: normalizes a value between a range aMin - aMax.
		static double normalize(double aValue, double aMin, double aMax);

		// helper function: adds a small random noise to a value and returns it.
		static double noise(double input, double epsilon, double random);

		// setter for the depth of this node and all nodes in the sub-tree this is root of.
		void setDepth(int depth);

		// Increments the counter of nodes below this one.
		void increaseTreeSize();


		//Applies an action "action" to a game state "gameState", using "forwardModel". It updates "params" for budget concerns.
		void applyActionToGameState(ForwardModel& forwardModel, GameState& targetGameState, Action& action, MCTSParameters& params, int playerID) const;

	public:
		// Root Node Constructor
		MCTSNode(ForwardModel& forwardModel, GameState gameState, int ownerID);

		/// <summary>
		/// Function for the main iteration of the MCTS algorithm
		/// </summary>
		void searchMCTS(ForwardModel& forwardModel, MCTSParameters& params, boost::mt19937& randomGenerator);

		/// <summary>
		/// Returns the index of the most visited action of this node.
		/// </summary>
		int mostVisitedAction(MCTSParameters& params, boost::mt19937& randomGenerator);

		/// <summary>
		/// Prints information of this node.
		/// </summary>
		void print() const override;

        void printActionInfo() const;

	private:

		// Creates a new node to be inserted in the tree.
		MCTSNode(ForwardModel& forwardModel, GameState gameState, MCTSNode* parent, int childIndex, int ownerID);

	};
}