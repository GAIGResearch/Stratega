#pragma once
#include <Stratega/Agent/TreeSearchAgents/MCTSParameters.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>

namespace SGA {

	class MCTSNode : public ITreeNode<MCTSNode>
	{
	public:
		int nodeDepth = 0;
		int treesize = 1;

	protected:
		int nVisits = 0;
		double bounds[2] = {0, 1};// {numeric_limits<double>::min(), numeric_limits<double>::max()};
				

	public:
		
		void initializeNode();
		void increaseTreeSize();

		// tree policy phase
		MCTSNode* treePolicy(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator);
		MCTSNode* expand(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator);
		MCTSNode* uct(MCTSParameters& params, std::mt19937& randomGenerator);

		// rollout phase
		double rollOut(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator);
		static bool rolloutFinished(GameState& rollerState, int depth, MCTSParameters& params);

		// backpropagation phase
		static void backUp(MCTSNode* node, double result);

		// return action
		int bestAction(MCTSParameters& params, std::mt19937& randomGenerator);

		// helper functions
		static double normalize(double aValue, double aMin, double aMax);
		static double noise(double input, double epsilon, double random);
		void applyActionToGameState(ForwardModel& forwardModel, GameState& targetGameState, Action& action, MCTSParameters& params, int playerID) const;
		void setDepth(int depth);

	public:
		// Root Node Constructor
		MCTSNode(ForwardModel& forwardModel, GameState gameState, int ownerID);

		//void setRootGameState(shared_ptr<TreeNode> root);
		void searchMCTS(ForwardModel& forwardModel, MCTSParameters& params, std::mt19937& randomGenerator);
		int mostVisitedAction(MCTSParameters& params, std::mt19937& randomGenerator);
		void print() const override;

	private:
		MCTSNode(ForwardModel& forwardModel, GameState gameState, MCTSNode* parent, int childIndex, int ownerID);
		
	};
}