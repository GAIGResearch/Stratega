#pragma once
#include <Stratega/Agent/StateAbstraction/IAbstractTreeNode.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA {

	class AbstractTreeNode : public IAbstractTreeNode<AbstractTreeNode>
	{
		AbstractTreeNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState, AbstractTreeNode* parent, int childIndex);

	public:
		// Root Node Constructor
		AbstractTreeNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState);
		AbstractTreeNode* expand(TBSForwardModel& forwardModel, AgentParameters& agentParameters);

		//std::string toString() const override;
		void print() const override;
	};

}