#pragma once
#include <Agent/Agent.h>
#include <Agent/Heuristic/UnitEvaluator.h>

namespace SGA
{
	class CombatAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
	private:
		UnitTypeStrengthLookup unitScores;

		/// <summary>
		/// Plays actions until the agent's turn is over
		/// </summary>
		void playTurn(TBSGameCommunicator& communicator, TBSForwardModel& fm) const;

		/// <summary>
		/// Filters out all moves that are not within the specified radius.
		/// </summary>
		std::vector<Action<Vector2i>> filterMovesInRange(const std::vector<SGA::Action<Vector2i>>& moves, Vector2i position, int range) const;

		/// <summary>
		/// Filters out all units that can't reach the specified position within one turn.
		/// </summary>
		std::vector<TBSUnit*> filterUnitsByReach(const std::vector<TBSUnit*>& targetUnits, const Vector2i& pos) const;

		/// <summary>
		/// Returns an estimate of how much healing one could receive when standing on an given position.
		/// </summary>
		int getPotentialHealing(const Vector2i& pos, const std::vector<TBSUnit*>& potentialHealers) const;

		/// <summary>
		/// Returns an estimate of how much damage one could receive when standing on an given position.
		/// </summary>
		double getPotentialDamage(const Vector2i& pos, const std::vector<TBSUnit*>& potentialAttackers) const;


		/// <summary>
		/// A heuristic to estimate how good an attack action is.
		/// </summary>
		double getAttackScore(const TBSUnit& target, const Action<Vector2i>& attack, const std::vector<TBSUnit*>& opponentUnits) const;

		/// <summary>
		/// A heuristic to estimate how good an healing action is.
		/// </summary>
		double getHealScore(const TBSUnit& target, const Action<Vector2i>& heal, const std::vector<TBSUnit*>& opponentUnits) const;
		
		/// <summary>
		/// Returns the best move for a unit to reach an target within an specified radius.
		/// If the unit can't reach the target immediately, this function will return a move that gets close to the target.
		/// The action will be stored in the bucket, the returned boolean indicates if a move was necessary to reach the target
		/// </summary>
		bool getMoveInRange(TBSUnit& u, const Vector2i& pos, int range, const std::vector<TBSUnit*>& opponentUnits, std::vector<SGA::Action<Vector2i>>& moves, Action<Vector2i>& bucket) const;
		
	};
}