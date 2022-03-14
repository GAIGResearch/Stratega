#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/AgentParameters.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>
#include <list>
#include <memory>

namespace SGA {



struct MinimaxParameters: public AgentParameters {
   //MinimaxParameters() {}

   bool continuePreviousSearch = true;
   int PLAYERID;
   int depth_limit;
   std::shared_ptr< StateHeuristic > heuristic;
};

// For turn-based deterministic perfect-information game.
class MinimaxAgent: public Agent {
   using Agent::Agent;

   int previousActionIndex = -1;

   MinimaxParameters parameters_ = MinimaxParameters();

  public:
   ActionAssignment computeAction(
      GameState state, const ForwardModel& forwardModel, Timer timer) override;

  private:
   void init(GameState initialState, const ForwardModel& forwardModel, Timer timer) override;

   double _alpha_beta(GameState state, const ForwardModel& forwardModel, int depth, double alpha,
      double beta, std::shared_ptr< StateHeuristic > heuristic, int PLAYER_ID);

};
}  // namespace SGA
