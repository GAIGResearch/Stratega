#pragma once
#include <Stratega/Agent/StateAbstraction/AbstractMCTSParameters.h>
#include <Stratega/Agent/StateAbstraction/AbstractTreeNode.h>

namespace SGA {

	class AbstractMCTSNode : public IAbstractTreeNode<AbstractMCTSNode>
	{
	public:
		int nodeDepth = 0;
		int treesize = 1;

	protected:
		int nVisits = 0;
		double bounds[2] = { 0, 1 };// {numeric_limits<double>::min(), numeric_limits<double>::max()};

	public:

		void initializeNode();
		void increaseTreeSize();

		// tree policy phase
		AbstractMCTSNode* treePolicy(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator);
		AbstractMCTSNode* expand(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator);
		AbstractMCTSNode* uct(AbstractMCTSParameters& params, std::mt19937& randomGenerator);

		// rollout phase
		double rollOut(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator);
		static bool rolloutFinished(GameState& rollerState, int depth, AbstractMCTSParameters& params);

		// backpropagation phase
		static void backUp(AbstractMCTSNode* node, double result);

		// return action
		int bestAction(AbstractMCTSParameters& params, std::mt19937& randomGenerator);

		// helper functions
		static double normalize(double aValue, double aMin, double aMax);
		static double noise(double input, double epsilon, double random);
		void applyActionToGameState(TBSForwardModel& forwardModel, GameState& gameState, Action& action, AbstractMCTSParameters& params) const;
		void setDepth(int depth);

	public:
		// Root Node Constructor
		AbstractMCTSNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState);

		//void setRootGameState(shared_ptr<TreeNode> root);
		void searchMCTS(TBSForwardModel& forwardModel, AbstractMCTSParameters& params, std::mt19937& randomGenerator);
		int mostVisitedAction(AbstractMCTSParameters& params, std::mt19937& randomGenerator);
		int getActionOfHighestRatedState(AbstractMCTSParameters& params, std::mt19937& randomGenerator);
		void print() const override;

	private:
		AbstractMCTSNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState, AbstractMCTSNode* parent, int childIndex);

	};
}