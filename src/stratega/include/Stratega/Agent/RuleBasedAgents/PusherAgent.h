#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/RuleBasedAgents/VectorHash2i.h>
#include <Stratega/Agent/RuleBasedAgents/Direction.h>
#include <Stratega/Agent/Heuristic/UnitEvaluator.h> 
#include <vector>
#include <queue>
#include <map>
#pragma warning(disable: 4365)
#pragma warning(disable: 4267)

namespace SGA
{
	class PusherAgent : public Agent
	{
	public:
		using Agent::Agent;
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;
		void init(GameState initialState, const ForwardModel& forwardModel, Timer timeBudgetMs) override;

        bool isInitialized = false;
        std::vector<Vector2f> hole_pos_list = {};

	private:
        boost::random::uniform_real_distribution<> dis = boost::random::uniform_real_distribution<> (0.0, 1.0);

		UnitTypeStrengthLookup unitScores;
		std::map<int, std::string> actionTypeIDToActionTypeString;

        void initialize(GameState state);

		/// <summary>
		/// Analyzes the given path to attack an target unit.
		/// </summary>
		/// <param name="pathCostOutput">Will be used to ouput a heuristic, for indicating how much it costs to move along this path</param>
		/// <returns>If it is safe to attack the unit</returns>
		bool analyzePath(GameState& state, Entity& target, const std::vector<Entity>& opponentUnits, const std::vector<Vector2i>& path, int pushCount, double& pathCostOutput) const;

		/// <summary>
		/// Finds the shortest paths to all given targets.
		/// The returned vector has the same length as "targets", and the path at index i corresponds to the shortest path to targets[i].
		/// The returned pair contains a boolean to indicate if a path was found, and the actual path.
		/// </summary>
		std::vector<std::pair<bool, std::vector<Vector2i>>> ShortestPaths(const GameState& state, const Vector2i& from, const std::vector<Vector2i>& targets) const;

		/// <summary>
		/// Assuming a unit stands at "pos", this function returns all directions from which another player could kill this unit.
		/// </summary>
		std::vector<Direction> GetDangerousDirections(const GameState& state, const Vector2i& pos) const;

		/// <summary>
		/// Assuming a unit stands at "pos", returns a mapping from direction to count.
		/// This tells you how many pushes into a direction are necessary to push the unit into an hole.
		/// </summary>
		std::unordered_map<Direction, int> getAttackDirections(const GameState& state, const Vector2i& pos) const;

		/// <summary>
		/// Checks if the tile would kill a entity.
		/// </summary>
		bool canKill(const GameState& state, Vector2i pos) const;

		/// <summary>
		/// Return all the actions from a entity of a list of actions.
		/// </summary>
		std::vector<Action> filterUnitActions(std::vector<Action>& actions, Entity& unit) const;

		/// <summary>
		/// Return all the actions from specific type.
		/// </summary>
		std::vector<Action> filterActionTypes(std::vector<Action>& actions, std::string type) const;

		/// <summary>
		/// Resturn the best action for the turn.
		/// </summary>
		ActionAssignment playTurn(GameState& newState, const ForwardModel& forwardModel);
	};
}