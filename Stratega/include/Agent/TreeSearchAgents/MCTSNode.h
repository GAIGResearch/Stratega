#pragma once
#include <Agent/TreeSearchAgents/MCTSParams.h>
#include <Agent/TreeSearchAgents/TreeNode.h>
#include <ForwardModel/Action.h>

namespace SGA {

	class MCTSNode : public ITreeNode<MCTSNode> {
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
		MCTSNode* treePolicy(TBSForwardModel& forwardModel, MCTSParams& params, std::mt19937& randomGenerator);
		MCTSNode* expand(TBSForwardModel& forwardModel, MCTSParams& params, std::mt19937& randomGenerator);
		MCTSNode* uct(MCTSParams& params, std::mt19937& randomGenerator);

		// rollout phase
		double rollOut(TBSForwardModel& forwardModel, MCTSParams& params, std::mt19937& randomGenerator);
		static bool rolloutFinished(TBSGameState& rollerState, int depth, MCTSParams& params);

		// backpropagation phase
		static void backUp(MCTSNode* node, double result);

		// return action
		int bestAction(MCTSParams& params, std::mt19937& randomGenerator);

		// helper functions
		static double normalize(double aValue, double aMin, double aMax);
		static double noise(double input, double epsilon, double random);
		void applyActionToGameState(TBSForwardModel& forwardModel, TBSGameState& gameState, Action<Vector2i>& action, MCTSParams& params) const;
		void setDepth(int depth);

	public:
		// Root Node Constructor
		MCTSNode(TBSForwardModel& forwardModel, TBSGameState gameState);

		//void setRootGameState(shared_ptr<TreeNode> root);
		void searchMCTS(TBSForwardModel& forwardModel, MCTSParams& params, std::mt19937& randomGenerator);
		int mostVisitedAction(MCTSParams& params, std::mt19937& randomGenerator);
		void print() const override;

	private:
		MCTSNode(TBSForwardModel& forwardModel, TBSGameState gameState, MCTSNode* parent, int childIndex);
		
	};
}