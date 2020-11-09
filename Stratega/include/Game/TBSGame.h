#pragma once
#include <mutex>
#include <Game/Game.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSForwardModel.h>
#include <random>

namespace SGA
{
	class TBSGame final : public Game
	{
	public:
		Action<Vector2i> actionToExecute;
		bool hasActionToExecute = false;
		
		TBSGame(std::unique_ptr<TBSGameState> gameState, TBSForwardModel forwardModel, std::mt19937 engine);

		void executeAction(Action<Vector2i> action);
		void update(double deltaTime) override;
		void close() override;
		bool isGameOver() const override { return Game::isGameOver() || gameState->isGameOver; }
		void addActionToExecute(Action<Vector2i> action);

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
