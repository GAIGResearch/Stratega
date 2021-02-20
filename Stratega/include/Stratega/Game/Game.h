#pragma once
#include <Stratega/Game/GameCommunicator.h>
#include <vector>
#include <memory>
#include <random>

namespace SGA
{
	class Game 
	{
	public:
		Game(std::mt19937 rngEngine);
		virtual ~Game() = default;
		
		virtual void init();
		virtual void close();
		virtual void run();
		virtual void update(double deltaTime) = 0;
		virtual void end() { endGame = true; };
		virtual bool isGameOver() const { return endGame; };

		virtual void addCommunicator(std::shared_ptr<GameCommunicator> comm);

		void stop() { isRunning = false; }
		void resume() { isRunning = true; }
		[[nodiscard]] bool running() const { return isRunning && !endGame; }
	protected:
		bool endGame = false;
		bool isRunning = true;

		std::mt19937 rngEngine;
		
		std::vector<std::shared_ptr<GameCommunicator>> communicators;
	};
}
