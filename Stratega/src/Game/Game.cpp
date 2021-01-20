#include <Game/Game.h>
#include <chrono>

namespace  SGA
{
	Game::Game(std::mt19937 rngEngine)
		: rngEngine(rngEngine)
	{
	}
	
	void Game::run()
	{
		init();
		auto lastFrameTime = std::chrono::high_resolution_clock::now();
		while(!isGameOver())
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> deltaTime = now - lastFrameTime;
			update(deltaTime.count());
			
			// Pause game
			while (!isRunning && !isGameOver());

			lastFrameTime = now;
		}
		close();
	}
	
	void Game::init()
	{
		for (auto& comm : communicators)
		{
			comm->init();
		}
	}

	void Game::close()
	{
		for (auto& comm : communicators)
		{
			comm->close();
		}
	}
	void Game::addCommunicator(std::shared_ptr<GameCommunicator> comm)
	{
		communicators.emplace_back(std::move(comm));
	}
}
