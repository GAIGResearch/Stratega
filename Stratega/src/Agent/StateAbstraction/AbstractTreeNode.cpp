#include <Stratega/Agent/StateAbstraction/AbstractTreeNode.h>
#include <iostream>
#include <vector>

namespace SGA
{

	AbstractTreeNode::AbstractTreeNode(TBSForwardModel& forwardModel, AbstractState abstractState, TBSGameState gameState) :
		IAbstractTreeNode<AbstractTreeNode>(forwardModel, std::move(abstractState), std::move(gameState))
	{
	}

	AbstractTreeNode::AbstractTreeNode(TBSForwardModel& forwardModel, AbstractState abstractState, TBSGameState gameState, AbstractTreeNode* parent, const int childIndex) :
		IAbstractTreeNode<AbstractTreeNode>(forwardModel, std::move(abstractState), std::move(gameState), parent, childIndex)
	{
	}


	/// <summary>
	/// Expanding the node with the next children. May fail in case the node is fully expanded.
	/// </summary>
	/// <param name="forwardModel"></param>
	/// <param name="agentParameters"></param>
	/// <returns></returns>
	AbstractTreeNode* AbstractTreeNode::expand(TBSForwardModel& forwardModel, AgentParameters& agentParameters)
	{
		if (this->isFullyExpanded())
			return nullptr;

		// roll the state using a the next action that hasn't been expanded yet
		auto gsCopy(gameState);
		forwardModel.advanceGameState(gsCopy, actionSpace.at(static_cast<int>(children.size())));
		agentParameters.REMAINING_FM_CALLS--;

		while (gsCopy.currentPlayer != agentParameters.PLAYER_ID && !gsCopy.isGameOver)
		{
			auto actionSpace = forwardModel.generateActions(gsCopy);
			auto opAction = agentParameters.OPPONENT_MODEL->getAction(gsCopy, actionSpace);
			forwardModel.advanceGameState(gsCopy, opAction);
			agentParameters.REMAINING_FM_CALLS--;
		}
		AbstractState abstractState = agentParameters.STATE_FACTORY->createAbstractState(gsCopy);

		children.emplace_back(std::unique_ptr<AbstractTreeNode>(new AbstractTreeNode(forwardModel, std::move(abstractState), std::move(gsCopy), this, children.size())));
		return children[children.size() - 1].get();
	}

	void AbstractTreeNode::print() const
	{
		std::cout << children.size();
	}
}
