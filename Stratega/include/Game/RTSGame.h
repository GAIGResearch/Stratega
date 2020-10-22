#pragma once
#include <Game/Game.h>
#include <Representation/RTSGameState.h>
#include <ForwardModel/RTSForwardModel.h>

#include <memory>
#include <mutex>
#include <random>

namespace SGA
{
	class RTSGame final : public Game
	{
	public:
		RTSGame(std::unique_ptr<RTSGameState> gameState, RTSForwardModel forwardModel, std::mt19937 engine);

		void update(double deltaTime) override;
		bool isGameOver() override { return Game::isGameOver() || gameState->isGameOver; };

		const RTSForwardModel& getForwardModel() const
		{
			return forwardModel;
		}

		void addAction(const Action<Vector2f>& action);

		/// <summary>
		/// Returns a reference to the internal gameState.
		/// Use this only to access scalar-variables, because the state is continuously modified
		/// For more complicated code use getStateCopy.
		/// </summary>
		[[nodiscard]] const RTSGameState& getState();
		[[nodiscard]] RTSGameState getStateCopy();

	private:
		std::vector<Action<Vector2f>> actionCache;
		double accumulatedTimeUpdate = 0;
		double accumulatedTimePrint = 0;
		int executionCount = 0;
	public:
		std::unique_ptr<RTSGameState> gameState;
		RTSForwardModel forwardModel;
		std::mutex stateMutex;
	};
}
