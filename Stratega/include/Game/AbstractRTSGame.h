#pragma once
#include <mutex>
#include <Game/Game.h>
#include <Representation/AbstractGS/RTSGameState2.h>
#include <ForwardModel/AbstractFM/RTSAbstractForwardModel.h>
#include <random>
namespace SGA
{
	class AbstractRTSGame final : public Game
	{
	private:
		double accumulatedTimeUpdate = 0;
		double accumulatedTimePrint = 0;
		int executionCount = 0;
		
	public:
		Action actionToExecute;
		bool hasActionToExecute = false;

		AbstractRTSGame(std::unique_ptr<RTSGameState2> gameState, RTSAbstractForwardModel forwardModel, std::mt19937 engine);

		void executeAction(Action action);
		void update(double deltaTime) override;
		void close() override;
		bool isGameOver() const override { return Game::isGameOver() || gameState->isGameOver; }
		
		const RTSAbstractForwardModel& getForwardModel() const { return forwardModel; }

		/// <summary>
		/// Returns a reference to the internal gameState.
		/// Use this only to access scalar-variables, because the state is continuously modified
		/// For more complicated code use getStateCopy.
		/// </summary>
		const RTSGameState2& getState() const;
		RTSGameState2 getStateCopy();

		[[nodiscard]] bool isUpdatingState() const { return updatingState && !isGameOver(); }

	private:
		bool updatingState = false;

		std::unique_ptr<RTSGameState2> gameState;
		RTSAbstractForwardModel forwardModel;

		std::mt19937 rngEngine;

		std::mutex stateMutex;
	public:
		//Navmesh Update
		NavigationConfig navigationConfig;
		bool shouldUpdateNavmesh;

	};
}
