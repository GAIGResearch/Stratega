#include "Stratega/Agent/TreeSearchAgents/Transition.h"

namespace SGA {

int unitStateHash(EntityForwardModel& forwardModel, GameState state, Entity entity)
{
   // get entity type
   int eID = entity.typeID;

   // get entity position
   int x = entity.position.x, y = entity.position.y;
   int base = 1000000;

   // return hash
   return base + x * 10000 + y * 100 + eID;
}

int unitActionHash(Action action)
{
   // endTurn action
   if(action.actionTypeFlags == ActionFlag::EndTickAction) {
      return 0;
   } else {
      // move and attack: 10000 + x*100 + y for move, attack: 100+targetID
      // action.actionTypeID
      int attackBase = 100;
      int moveBase = 10000;

      if(action.targets[1].getType() == ActionTarget::Type::Position) {
         return moveBase + action.targets[1].data.position.x * 100
                + action.targets[1].data.position.y;
      } else if(action.targets[1].getType() == ActionTarget::Type::EntityReference) {
         int eID = action.targets[1].data.entityID;
         return attackBase + eID;
      } else {
         std::cout << "[actionHahs] action type unknown" << std::endl;
      }
   }
}

/*
// two nodes are responsible for the same unit type
bool isTwoNodeApproxmateHomomorphism(UnitMCTSNode node1, UnitMCTSNode node2, double
reward_threshold, double transition_threshold) {

        // for all action, reward situation
        double max_reward_diff = 10.0;



        return false;
}
*/

Transition::Transition()
{
   transition_mapping = std::map< int, std::vector< std::pair< int, int > > >();
   reward_mapping = std::map< std::pair< int, int >, double >();
}

void Transition::addTransition(int stateHash, int actionHash, int nextStateHash, double reward)
{
   transition_mapping[stateHash].push_back(std::make_pair(actionHash, nextStateHash));
   std::pair SApair = std::make_pair(stateHash, nextStateHash);
   // TODO
   // reward_mapping[SApair].push_back(reward);
}

/* each depth has a StateMapping
 */
StateMapping::StateMapping()
{
   stateMapping = std::map< int, std::vector< int > >();
   wasAbstracted = std::map< int, bool >();
}

bool StateMapping::existMapping(int stateHash)
{
   if(wasAbstracted.count(stateHash)) {
      return true;
   } else {
      return false;
   }
}

}  // namespace SGA