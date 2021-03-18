#pragma once
#include <Stratega/Agent/TreeSearchAgents/ITreeNode.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {

	class TreeNode : public ITreeNode<TreeNode>
	{
		TreeNode(ForwardModel& forwardModel, GameState gameState, TreeNode* parent, int childIndex);

	public:
		// Root Node Constructor
		TreeNode(ForwardModel& forwardModel, GameState gameState);
		TreeNode* expand(ForwardModel& forwardModel, AgentParameters& agentParameters);
		
		//std::string toString() const override;
		void print() const override;
	};	

}
