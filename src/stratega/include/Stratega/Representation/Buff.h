#pragma once
#include <unordered_map>
#include <vector>
#include <Stratega/Representation/BuffType.h>
namespace SGA {
    //struct BuffType;
    struct Buff {
        private:
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
        static Buff createBuff(const BuffType& type, int entityID, int durationTicks)
        {
           Buff b;
           b.ownerID = entityID;
           b.type = &type;
           b.durationTicks = durationTicks;
           b.elapsedTicks = 0;
           return b;
        }

        /// <summary>
        /// Return the buff type
        /// </summary>
        /// <returns>Buff type</returns>
        const BuffType& getType() const 
        { 
            return *type;
        }

        /// <summary>
        /// Return the number of ticks elapsed of this buff
        /// </summary>
        /// <returns>Elapsed ticks</returns>
        int getElapsedTicks() const { return elapsedTicks; }

        /// <summary>
        /// Apply the new amount of elapsed ticks this buff has
        /// </summary>
        /// <param name="newElapsedTicks">New amount of elapsed ticks</param>
        void setElapsedTicks(int newElapsedTicks) { elapsedTicks = newElapsedTicks; }

        /// <summary>
        /// Return the number of duration ticks of this buff
        /// </summary>
        /// <returns>Duration ticks</returns>
        int getDurationTicks() const { return durationTicks; }

        /// <summary>
        /// Apply the new amount of duration ticks this buff has
        /// </summary>
        /// <param name="newElapsedTicks">New amount of duration ticks</param>
        void setDurationTicks(int newDurartionTicks) { durationTicks=newDurartionTicks; }

        /// <summary>
        /// Increment the elpased tick by one
        /// </summary>
        void incrementElapseTicks() { elapsedTicks++; }
    };
}  // namespace SGA