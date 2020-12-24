#pragma once
#include <mutex>
#include <Game/Game.h>
#include <Representation/TBSGameState.h>
#include <Representation/RTSGameState.h>
#include <ForwardModel/TBSAbstractForwardModel.h>
#include <ForwardModel/RTSAbstractForwardModel.h>
#include <random>

namespace SGA
{
	class AbstractTBSGame final : public Game
	{
	public:
		Action actionToExecute;
		bool hasActionToExecute = false;

		AbstractTBSGame(std::unique_ptr<TBSGameState2> gameState, TBSAbstractForwardModel forwardModel, std::mt19937 engine);

		void executeAction(Action action);
		void update(double deltaTime) override;
		void close() override;
		bool isGameOver() const override { return Game::isGameOver() || gameState->isGameOver; }
		void addActionToExecute(Action action);

		const TBSAbstractForwardModel& getForwardModel() const { return forwardModel; }

		/// <summary>
		/// Returns a reference to the internal gameState.
		/// Use this only to access scalar-variables, because the state is continuously modified
		/// For more complicated code use getStateCopy.
		/// </summary>
		const TBSGameState2& getState() const;
		TBSGameState2 getStateCopy();

		[[nodiscard]] bool isUpdatingState() const { return updatingState && !isGameOver(); }

	private:
		bool updatingState = false;

		std::unique_ptr<TBSGameState2> gameState;
		TBSAbstractForwardModel forwardModel;

		std::mt19937 rngEngine;

		std::mutex mutexGameState;
	};
}
