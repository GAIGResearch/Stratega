#pragma once
#include <mutex>
#include <Game/Game.h>
#include <Representation/AbstractGS/TBSGameState2.h>
#include <ForwardModel/AbstractFM/ForwardModel.h>
#include <random>

namespace SGA
{
	class AbstractGame final : public Game
	{
	public:
		Action actionToExecute;
		bool hasActionToExecute = false;

		AbstractGame(std::unique_ptr<TBSGameState2> gameState, ForwardModel<TBSGameState2> forwardModel, std::mt19937 engine);

		void executeAction(Action action);
		void update(double deltaTime) override;
		void close() override;
		bool isGameOver() const override { return Game::isGameOver() || gameState->isGameOver; }
		void addActionToExecute(Action action);

		const ForwardModel<TBSGameState2>& getForwardModel() const { return forwardModel; }

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
		ForwardModel<TBSGameState2> forwardModel;

		std::mt19937 rngEngine;

		std::mutex mutexGameState;
	};

}
