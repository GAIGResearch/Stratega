#pragma once
#include <Game/GameCommunicator.h>
#include <Game/AbstractRTSGame.h>
#include <random>
#include <thread>

namespace SGA
{
	class Agent;
	
	class AbstractRTSGameCommunicator : public GameCommunicator
	{
	public:
		AbstractRTSGameCommunicator(int playerID) :
			GameCommunicator{ playerID }
		{
		}

		void init() override;
		void close() override
		{
			thread.join();
		}
		bool isRunning = false;

		std::mt19937& getRNGEngine() { return rngEngine; }
		bool isGameOver() const;

		//TODO private
		void setGame(AbstractRTSGame& newGame);
		void setAgent(std::unique_ptr<Agent> agent);
		void setRNGEngine(std::mt19937 engine) { rngEngine = engine; }

		void executeAction(Action action) const;
		bool isMyTurn() const;
		RTSGameState2 getGameState() const;

	private:
		AbstractRTSGame* game;
		std::unique_ptr<Agent> agent;
		std::mt19937 rngEngine;
		std::thread thread;
	};
}
