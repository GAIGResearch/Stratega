#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/MinimaxAgent.h>

namespace SGA {

// alpha-beta search
ActionAssignment MinimaxAgent::computeAction(
   GameState state, const ForwardModel& forwardModel, Timer timer)
{
   //std::cout << "Starting MinimaxAgent" << std::endl;
   // parameters_.resetCounters(timer);
   parameters_.PLAYER_ID = getPlayerID();
   parameters_.depth_limit = 2;
   if (parameters_.heuristic == nullptr) {
      parameters_.heuristic = std::make_shared< AimToKingHeuristic >(state);  // check, if it is initialized here, at each step
   }
  

   const auto actionSpace = forwardModel.generateActions(state, getPlayerID());

   if(actionSpace.size() == 1) {
      return ActionAssignment::fromSingleAction(actionSpace.front());
   }

   double infinity = std::numeric_limits< double >::infinity();

   double bestActionID = _alpha_beta(
      state,
      forwardModel,
      /*depth=*/parameters_.depth_limit,
      /*alpha=*/-infinity,
      /*beta=*/infinity,
      parameters_.heuristic,
      parameters_.PLAYER_ID
   );
   auto action = actionSpace.at((int) bestActionID);

   //std::cout << "Finished MinimaxAgent" << std::endl;
   return ActionAssignment::fromSingleAction(action);
}

double MinimaxAgent::_alpha_beta(GameState state, const ForwardModel& forwardModel, int depth, double alpha, double beta, 
    std::shared_ptr< StateHeuristic > heuristic, int PLAYER_ID){
   if(depth == 0 || state.isGameOver()) {
      return heuristic->evaluateGameState(forwardModel, state, PLAYER_ID);
   }

   double value = -std::numeric_limits< double >::infinity();
   int bestActionID = -1;

   bool isMaxPlayer = state.canPlay(PLAYER_ID);
   int currentPlayer = (isMaxPlayer ? PLAYER_ID : (-PLAYER_ID + 1)); // 0-1 exchange

   const auto actionSpace = forwardModel.generateActions(state, currentPlayer);

   if(isMaxPlayer) {
      // for all actions, get a value, return maximum

      for(int action_idx = 0; action_idx < actionSpace.size(); action_idx++) {
         auto gsCopy(state);
         forwardModel.advanceGameState(gsCopy, actionSpace.at(action_idx));  // TODO check API

         double child_value = _alpha_beta(
            gsCopy, forwardModel, depth - 1, alpha, beta, heuristic, PLAYER_ID);

         if(child_value > value) {
            value = child_value;
            bestActionID = action_idx;
         }
         alpha = std::max(alpha, value);

         if(alpha >= beta) {
            break;  // beta cut-off
         }
      }
   } else {
      // for all actions, get a value, return minimum
      for(int action_idx = 0; action_idx < actionSpace.size(); action_idx++) {
         auto gsCopy(state);
         forwardModel.advanceGameState(gsCopy, actionSpace.at(action_idx));  // TODO check API

         double child_value = _alpha_beta(
            gsCopy, forwardModel, depth - 1, alpha, beta, heuristic, PLAYER_ID);

         if(child_value < value) {
            value = child_value;
            bestActionID = action_idx;
         }
         beta = std::min(beta, value);

         if(alpha >= beta) {
            break;  // beta cut-off
         }
      }
   }

   // usualy return a value, but return the bestActionID here for evlauation in real environment
   if(depth == 1) {
      return bestActionID;
   }
   return value;
}

void MinimaxAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
{
   parameters_.PLAYER_ID = getPlayerID();
   if(parameters_.heuristic == nullptr)
      parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
   if(parameters_.budgetType == Budget::UNDEFINED)
      parameters_.budgetType = Budget::TIME;
}

} 
