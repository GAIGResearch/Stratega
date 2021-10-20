#pragma once
#include <unordered_map>
#include <vector>

namespace SGA {

struct Buff {
  private:
   /// <summary>
   // ID of this buff in the game.
   /// <summary>
   int id;

   /// <summary>
   // ID of the entity who have this buff.
   /// <summary>
   int ownerID;

   int elapsedTicks;

  public:
  
};
}  // namespace SGA