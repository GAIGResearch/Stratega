#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>
#include <Stratega/Agent/Agent.h>
#include <iostream>
#include <vector>

namespace SGA
{

	TreeNode::TreeNode(ForwardModel& forwardModel, GameState gameState, int ownerID) :
		ITreeNode<SGA::TreeNode>(forwardModel, std::move(gameState), ownerID)
	{
	}

	TreeNode::TreeNode(ForwardModel& forwardModel, GameState gameState, TreeNode* parent, const int childIndex, int ownerID) :
		ITreeNode<SGA::TreeNode>(forwardModel, std::move(gameState), parent, childIndex, ownerID)
	{
	}


	/// <summary>
	/// Expanding the node with the next children. May fail in case the node is fully expanded.
	/// </summary>
	/// <param name="forwardModel"></param>
	/// <param name="agentParameters"></param>
	/// <returns></returns>
	TreeNode* TreeNode::expand(ForwardModel& forwardModel, AgentParameters& agentParameters)
	{		
		if (this->isFullyExpanded())
			return nullptr;

		// roll the state using a the next action that hasn't been expanded yet
		auto gsCopy(gameState);
		auto action = actionSpace.at(static_cast<int>(children.size()));
		agentParameters.currentFMCalls += SGA::roll(gsCopy, forwardModel, action, playerID, agentParameters);
				
		children.emplace_back(std::unique_ptr<TreeNode>(new TreeNode(forwardModel, std::move(gsCopy), this, static_cast<int>(children.size()), this->ownerID)));
		return children[children.size()-1].get();
	}

	void TreeNode::print() const
	{	
		std::cout << children.size();
	}
}
