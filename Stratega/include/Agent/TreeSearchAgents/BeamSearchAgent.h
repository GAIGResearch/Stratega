#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/TreeNode.h>
#include <Agent/AgentParameters.h>

namespace SGA
{
	struct BeamSearchParameters : AgentParameters
	{
		size_t PLAYER_BEAM_WIDTH = 20;
		size_t PLAYER_BEAM_DEPTH = 5;
	};
	
	class BeamSearchAgent : public Agent
	{
	private:
		BeamSearchParameters parameters_ = BeamSearchParameters();

	public:
		void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel) override;

	private:	
		Action beamSearch(TBSAbstractForwardModel& forwardModel, TreeNode& root);
		std::vector<TreeNode*> simulate(TBSAbstractForwardModel& forwardModel, TreeNode& node);
		static bool sortByValue(const TreeNode* i, const TreeNode* j);
	};
}
