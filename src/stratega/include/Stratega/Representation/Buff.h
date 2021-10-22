#pragma once
#include <unordered_map>
#include <vector>
#include <Stratega/Representation/BuffType.h>
namespace SGA {
    //struct BuffType;
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

        int durationTicks;

        const BuffType* type;
        public:

        /// <summary>
        /// Generates an Action used by the game to end the tick/turn.
        /// </summary>
        /// <param name="playerID">ID of the player for which the End action is created</param>
        /// <returns>Action that ends the current turn for the plater ID supplied</returns>
        static Buff createBuff(const BuffType& type, int entityID, int buffID, int durationTicks)
        {
           Buff b;
           b.ownerID = entityID;
           b.id = buffID;
           b.type = &type;
           b.durationTicks = durationTicks;
           b.elapsedTicks = 0;
           return b;
        }

        const BuffType& getType() const 
        { 
            return *type;
        }

        int getElapsedTicks() const { return elapsedTicks; }

        void setElapsedTicks(int newElapsedTicks) { elapsedTicks = newElapsedTicks; }

        int getDurationTicks() const { return durationTicks; }

        void setDurationTicks(int newDurartionTicks) { durationTicks=newDurartionTicks; }

        void incrementElapseTicks() { elapsedTicks++; }
    };
}  // namespace SGA