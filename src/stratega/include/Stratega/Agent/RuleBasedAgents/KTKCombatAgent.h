#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/UnitEvaluator.h>
#include <map>
#include <vector>

namespace SGA {
    class KTKCombatAgent : public Agent {
    public:
        using Agent::Agent;
        ActionAssignment computeAction(
            GameState state, const ForwardModel& forwardModel, Timer timer) override;
        void init(GameState initialState, const ForwardModel& forwardModel, Timer timeBudgetMs) override;

    private:
        UnitTypeStrengthLookup unitScores;
        std::map< int, std::string > actionTypeIDToActionTypeString;

        std::vector< Action > filterUnitActions(std::vector< Action >& actions, Entity& unit) const;
        std::vector< Action > filterActionTypes(std::vector< Action >& actions, std::string type) const;

        /// <summary>
        /// Plays actions until the agent's turn is over
        /// </summary>
        ActionAssignment playTurn(GameState& state, const ForwardModel& fm);

        /// <summary>
        /// Filters out all moves that are not within the specified radius.
        /// </summary>
        std::vector< Action > filterMovesInRange(
            const std::vector< SGA::Action >& moves,
            Vector2f position,
            int range,
            GameState& gameState) const;

        /// <summary>
        /// Filters out all units that can't reach the specified position within one turn.
        /// </summary>
        std::vector< Entity > filterUnitsByReach(
            const std::vector< Entity >& targetUnits, const Vector2f& pos, GameState& gameState) const;

        /// <summary>
        /// Returns an estimate of how much healing one could receive when standing on an given position.
        /// </summary>
        int getPotentialHealing(
            std::vector< Action >& actions,
            const Vector2f& pos,
            const std::vector< Entity >& potentialHealers,
            GameState& gameState) const;

        /// <summary>
        /// Returns an estimate of how much damage one could receive when standing on an given position.
        /// </summary>
        double getPotentialDamage(
            const Vector2f& pos,
            const std::vector< Entity >& potentialAttackers,
            GameState& gameState) const;

        /// <summary>
        /// A heuristic to estimate how good an attack action is.
        /// </summary>
        double getAttackScore(
            std::vector< Action >& actions,
            const Entity& target,
            const Action& attack,
            const std::vector< Entity >& opponentUnits,
            GameState& gameState) const;

        /// <summary>
        /// A heuristic to estimate how good an healing action is.
        /// </summary>
        double getHealScore(
            std::vector< Action >& actions,
            const Entity& target,
            const Action& heal,
            const std::vector< Entity >& opponentUnits,
            GameState& gameState) const;

        /// <summary>
        /// Returns the best move for a unit to reach an target within an specified radius.
        /// If the unit can't reach the target immediately, this function will return a move that gets
        /// close to the target. The action will be stored in the bucket, the returned boolean indicates
        /// if a move was necessary to reach the target
        /// </summary>
        bool getMoveInRange(
            Entity& u,
            const Vector2f& pos,
            int range,
            const std::vector< Entity >& opponentUnits,
            std::vector< SGA::Action >& moves,
            Action& bucket,
            GameState& gameState) const;

    };
}  // namespace SGA