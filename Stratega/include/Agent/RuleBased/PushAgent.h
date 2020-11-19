#pragma once
#include <Agent/Agent.h>
#include <Agent/RuleBased/Direction.h>

namespace SGA
{
	class PushAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		
	private:
		TBSForwardModel forwardModel;
		
		/// <summary>
		/// Analyzes the given path to attack an target unit.
		/// </summary>
		/// <param name="pathCostOutput">Will be used to ouput a heuristic, for indicating how much it costs to move along this path</param>
		/// <returns>If it is safe to attack the unit</returns>
		bool analyzePath(TBSGameState& state, TBSUnit& target, const std::vector<TBSUnit>& opponentUnits, const std::vector<Vector2i>& path, int pushCount, double& pathCostOutput) const;

		/// <summary>
		/// Finds the shortest paths to all given targets.
		/// The returned vector has the same length as "targets", and the path at index i corresponds to the shortest path to targets[i].
		/// The returned pair contains a boolean to indicate if a path was found, and the actual path.
		/// </summary>
		std::vector<std::pair<bool, std::vector<Vector2i>>> ShortestPaths(const TBSGameState& state, const Vector2i& from, const std::vector<Vector2i>& targets) const;

		/// <summary>
		/// Assuming a unit stands at "pos", this function returns all directions from which another player could kill this unit.
		/// </summary>
		std::vector<Direction> GetDangerousDirections(const TBSGameState& state, const Vector2i& pos) const;

		/// <summary>
		/// Assuming a unit stands at "pos", returns a mapping from direction to count.
		/// This tells you how many pushes into a direction are necessary to push the unit into an hole.
		/// </summary>
		std::unordered_map<Direction, int> getAttackDirections(const TBSGameState& state, const Vector2i& pos) const;
	};;
}