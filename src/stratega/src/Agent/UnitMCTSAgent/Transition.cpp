#include "Stratega/Agent/UnitMCTSAgent/Transition.h"

namespace SGA {

int unitStateHash(ForwardModel& forwardModel, GameState state, Entity entity)
{
   // get entity type
    int eID = entity.getEntityTypeID();//entity.typeID;

   // get entity position
   int x = entity.x(), y = entity.y();
   int base = 1000000;

   // return hash
   return base + x * 10000 + y * 100 + eID;
}

int unitActionHash(Action action)
{
   // endTurn action
   if(action.getActionFlag() == ActionFlag::EndTickAction) {
      return 0;
   } else {
      // move and attack: 10000 + x*100 + y for move, attack: 100+targetID
      // action.actionTypeID
      int attackBase = 100;
      int moveBase = 10000;
      auto& targets = action.getTargets();

      if(targets[1].getType() == ActionTarget::Type::Position) {
         return moveBase + targets[1].getPosition().x * 100
                + targets[1].getPosition().y;
      } else if(targets[1].getType() == ActionTarget::Type::EntityReference) {
         int eID = targets[1].getEntityID();
         return attackBase + eID;
      } else {
         std::cout << "[actionHahs] action type unknown" << std::endl;
      }
   }
}


Transition::Transition()
{
   transition_mapping = std::map< int, std::vector< std::pair< int, int > > >();
   reward_mapping = std::map< std::pair< int, int >, double >();
}

void Transition::addTransition(int stateHash, int actionHash, int nextStateHash, double reward)
{
   transition_mapping[stateHash].push_back(std::make_pair(actionHash, nextStateHash));
   std::pair<int, int> SApair = std::make_pair(stateHash, nextStateHash);
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