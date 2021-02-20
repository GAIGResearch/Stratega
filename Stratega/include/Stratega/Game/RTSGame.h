#pragma once
#include <mutex>
#include <Stratega/Game/Game.h>
#include <Stratega/Representation/RTSGameState.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <random>
namespace SGA
{
	class RTSGame final : public Game
	{
	private:
		double accumulatedTimeUpdate = 0;
		double accumulatedTimePrint = 0;
		int executionCount = 0;
		
	public:
		Action actionToExecute;
		bool hasActionToExecute = false;

		RTSGame(std::unique_ptr<RTSGameState> gameState, RTSForwardModel forwardModel, std::mt19937 engine);

		void executeAction(Action action);
		void update(double deltaTime) override;
		void close() override;
		bool isGameOver() const override { return Game::isGameOver() || gameState->isGameOver; }
		
		const RTSForwardModel& getForwardModel() const { return forwardModel; }

		/// <summary>
		/// Returns a reference to the internal gameState.
		/// Use this only to access scalar-variables, because the state is continuously modified
		/// For more complicated code use getStateCopy.
		/// </summary>
		const RTSGameState& getState() const;
		RTSGameState getStateCopy();

		[[nodiscard]] bool isUpdatingState() const { return updatingState && !isGameOver(); }

	private:
		bool updatingState = false;

		std::unique_ptr<RTSGameState> gameState;
		RTSForwardModel forwardModel;

		std::mt19937 rngEngine;

		std::mutex stateMutex;
	public:
		//Navmesh Update
		NavigationConfig navigationConfig;
		bool shouldUpdateNavmesh;

	};
}
