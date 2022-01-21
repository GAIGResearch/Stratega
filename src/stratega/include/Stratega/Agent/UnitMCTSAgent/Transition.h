#pragma once

#include <iostream>
#include <map>
#include <set>
#include <utility>

#include "Stratega/ForwardModel/Action.h"
#include "Stratega/ForwardModel/ForwardModel.h"
#include "Stratega/Representation/Entity.h"
#include "Stratega/Representation/GameState.h"
//#include "Stratega/Agent/TreeSearchAgents/UnitMCTSNode.h"

namespace SGA {
int unitStateHash(ForwardModel& forwardModel, GameState state, Entity entity);

int unitActionHash(Action action);

///*
struct UnitState {
   int unitID;
   int unitType;
   int x, y;

   // UnitState();
};

struct StateMapping {
   std::map< int, std::vector< int > > stateMapping;
   std::map< int, bool >
      wasAbstracted;  // state->bool, if bool=true, this state has a record being abstracted

   StateMapping();
   bool existMapping(int stateHash);
};

struct Transition {
   std::map< int, std::vector< std::pair< int, int > > >
      transition_mapping;  // thisState -> [action, nextState]
   std::map< std::pair< int, int >, double > reward_mapping;

   int actionType;  // 0:move, 1:attack, 2:endTurn
   std::vector< int > actionTarget;

   Transition();

   // be called right after expand
   void addTransition(int stateHash, int actionHash, int nextStateHash, double reward);
};

}  // namespace SGA
