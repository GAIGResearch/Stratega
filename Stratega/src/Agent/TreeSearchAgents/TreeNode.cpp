#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>
#include <iostream>
#include <vector>

namespace SGA
{

	TreeNode::TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState) :
		ITreeNode<SGA::TreeNode>(forwardModel, std::move(gameState))
	{
	}

	TreeNode::TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState, TreeNode* parent, const int childIndex) :
		ITreeNode<SGA::TreeNode>(forwardModel, std::move(gameState), parent, childIndex)
	{
	}
	
	
	/// <summary>
	/// Expanding the node with the next children. May fail in case the node is fully expanded.
	/// </summary>
	/// <param name="forwardModel"></param>
	/// <param name="agentParameters"></param>
	/// <returns></returns>
	TreeNode* TreeNode::expand(TBSForwardModel& forwardModel, AgentParameters& agentParameters)
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
		
		children.emplace_back(std::unique_ptr<TreeNode>(new TreeNode(forwardModel, std::move(gsCopy), this, children.size())));
		return children[children.size()-1].get();
	}

	void TreeNode::print() const
	{	
		std::cout << children.size();
	}
}
