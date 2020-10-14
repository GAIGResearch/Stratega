#include <Agent/TreeSearchAgents/TreeNode.h>
#include <iostream>
#include <vector>

namespace SGA
{

	TreeNode::TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState) :
		ITreeNode<SGA::TreeNode>(forwardModel, gameState)
	{
	}

	TreeNode::TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState, TreeNode* parent, const int childIndex) :
		ITreeNode<SGA::TreeNode>(forwardModel, gameState, parent, childIndex)
	{
	}
	
	
	/// <summary>
	/// Expanding the node with the next children. May fail in case the node is fully expanded.
	/// </summary>
	/// <param name="forwardModel"></param>
	/// <returns></returns>
	TreeNode* TreeNode::expand(TBSForwardModel& forwardModel)
	{
		//std::cout << "expand" << std::endl;
		
		if (this->isFullyExpanded())
			return nullptr;

		// roll the state using a the next action that hasn't been expanded yet
		TBSGameState gsCopy = TBSGameState(gameState);
		forwardModel.advanceGameState(gsCopy, actionSpace->getAction(static_cast<int>(children.size())));
		while (gsCopy.currentPlayer != gameState.currentPlayer && !gameState.isGameOver)
		{
			ActionSpace<Vector2i> endTurnActionSpace;
			forwardModel.generateEndOfTurnActions(gameState, gameState.currentPlayer, endTurnActionSpace);
			forwardModel.advanceGameState(gameState, endTurnActionSpace.getAction(0));
		}
		
		children.emplace_back(std::unique_ptr<TreeNode>(new TreeNode(forwardModel, std::move(gsCopy), this, children.size())));
		return children[children.size()-1].get();
	}

	void TreeNode::print() const
	{	
		std::cout << children.size();
	}
}
