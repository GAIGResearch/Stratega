#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>
#include <Stratega/Agent/AgentParameters.h>

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
		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) override;
		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;

	private:	
		Action beamSearch(ForwardModel& forwardModel, TreeNode& root);
		std::vector<TreeNode*> simulate(ForwardModel& forwardModel, TreeNode& node);
		static bool sortByValue(const TreeNode* i, const TreeNode* j);
	};
}
