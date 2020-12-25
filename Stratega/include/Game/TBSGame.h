#pragma once
#include <mutex>
#include <Game/Game.h>
#include <Representation/TBSGameState.h>
#include <Representation/RTSGameState.h>
#include <ForwardModel/TBSForwardModel.h>
#include <ForwardModel/RTSForwardModel.h>
#include <random>

namespace SGA
{
	class TBSGame final : public Game
	{
	public:
		Action actionToExecute;
		bool hasActionToExecute = false;

		TBSGame(std::unique_ptr<TBSGameState> gameState, TBSForwardModel forwardModel, std::mt19937 engine);

		void executeAction(Action action);
		void update(double deltaTime) override;
		void close() override;
		bool isGameOver() const override { return Game::isGameOver() || gameState->isGameOver; }
		void addActionToExecute(Action action);

		const TBSForwardModel& getForwardModel() const { return forwardModel; }

		/// <summary>
		/// Returns a reference to the internal gameState.
		/// Use this only to access scalar-variables, because the state is continuously modified
		/// For more complicated code use getStateCopy.
		/// </summary>
		const TBSGameState& getState() const;
		TBSGameState getStateCopy();

		[[nodiscard]] bool isUpdatingState() const { return updatingState && !isGameOver(); }

	private:
		bool updatingState = false;

		std::unique_ptr<TBSGameState> gameState;
		TBSForwardModel forwardModel;

		std::mt19937 rngEngine;

		std::mutex mutexGameState;
	};
}
