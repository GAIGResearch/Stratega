#pragma once
#include <Agent/TreeSearchAgents/ITreeNode.h>
#include <Agent/AgentParameters.h>

namespace SGA {

	class TreeNode : public ITreeNode<TreeNode>
	{
		TreeNode(TBSAbstractForwardModel& forwardModel, TBSGameState2 gameState, TreeNode* parent, int childIndex);

	public:
		// Root Node Constructor
		TreeNode(TBSAbstractForwardModel& forwardModel, TBSGameState2 gameState);
		TreeNode* expand(TBSAbstractForwardModel& forwardModel, AgentParameters& agentParameters);
		
		//std::string toString() const override;
		void print() const override;
	};	

}
