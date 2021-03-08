#pragma once
#include <Stratega/Agent/Agent.h>
#include <vector>
#include <map>


namespace SGA
{
	class CombatAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
	private:
		//UnitTypeStrengthLookup unitScores;
		TBSGameState* currentGameState;
		std::map<int, std::string> actionTypeIDToActionTypeString;

		std::vector<Action> filterUnitActions(std::vector<Action>& actions, Entity& unit) const;
		std::vector<Action> filterActionTypes(std::vector<Action>& actions, std::string type) const;

		/// <summary>
		/// Plays actions until the agent's turn is over
		/// </summary>
		void playTurn(TBSGameCommunicator& communicator, TBSForwardModel& fm);

		/// <summary>
		/// Filters out all moves that are not within the specified radius.
		/// </summary>
		std::vector<Action> filterMovesInRange(const std::vector<SGA::Action>& moves, Vector2i position, int range) const;

		/// <summary>
		/// Filters out all units that can't reach the specified position within one turn.
		/// </summary>
		std::vector<Entity*> filterUnitsByReach(const std::vector<Entity*>& targetUnits, const Vector2i& pos) const;

		/// <summary>
		/// Returns an estimate of how much healing one could receive when standing on an given position.
		/// </summary>
		int getPotentialHealing(const Vector2i& pos, const std::vector<Entity*>& potentialHealers) const;

		/// <summary>
		/// Returns an estimate of how much damage one could receive when standing on an given position.
		/// </summary>
		double getPotentialDamage(const Vector2i& pos, const std::vector<Entity*>& potentialAttackers) const;


		/// <summary>
		/// A heuristic to estimate how good an attack action is.
		/// </summary>
		double getAttackScore(const Entity& target, const Action& attack, const std::vector<Entity*>& opponentUnits) const;

		/// <summary>
		/// A heuristic to estimate how good an healing action is.
		/// </summary>
		double getHealScore(const Entity& target, const Action& heal, const std::vector<Entity*>& opponentUnits) const;

		/// <summary>
		/// Returns the best move for a unit to reach an target within an specified radius.
		/// If the unit can't reach the target immediately, this function will return a move that gets close to the target.
		/// The action will be stored in the bucket, the returned boolean indicates if a move was necessary to reach the target
		/// </summary>
		bool getMoveInRange(Entity& u, const Vector2i& pos, int range, const std::vector<Entity*>& opponentUnits, std::vector<SGA::Action>& moves, Action& bucket) const;

	};
}
