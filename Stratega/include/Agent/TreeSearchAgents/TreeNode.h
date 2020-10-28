#pragma once
#include <memory>
#include <vector>


#include <Agent/TreeSearchAgents/ITreeNode.h>
#include <ForwardModel/TBSForwardModel.h>
#include <Representation/TBSGameState.h>

#include "Agent/ActionScripts/BaseActionScript.h"


namespace SGA {

	class TreeNode : public ITreeNode<TreeNode> {
		
	private:
		TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState, TreeNode* parent, int childIndex);

	public:
		// Root Node Constructor
		TreeNode(TBSForwardModel& forwardModel, TBSGameState gameState);
		TreeNode* expand(TBSForwardModel& forwardModel, BaseActionScript* opponentModel, int& forwardModelCalls);
		
		//std::string toString() const override;
		void print() const override;
	};	

}
