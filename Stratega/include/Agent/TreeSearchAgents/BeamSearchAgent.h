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
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:	
		Action beamSearch(TBSForwardModel& forwardModel, TreeNode& root);
		std::vector<TreeNode*> simulate(TBSForwardModel& forwardModel, TreeNode& node);
		static bool sortByValue(const TreeNode* i, const TreeNode* j);
	};
}
