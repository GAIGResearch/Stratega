#pragma once
#include <Game/GameCommunicator.h>
#include <Representation/AbstractGS/TBSGameState2.h>
#include <Game/AbstractGame.h>
#include <Agent/Agent.h>

#include <random>
#include <thread>

namespace SGA
{
	class Agent;

	class AbstractTBSGameCommunicator : public GameCommunicator
	{
	public:
		AbstractTBSGameCommunicator(int playerID) :
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
		void setGame(AbstractTBSGame& newGame);
		void setAgent(std::unique_ptr<Agent> agent);
		void setRNGEngine(std::mt19937 engine) { rngEngine = engine; }

		void executeAction(Action action) const;
		bool isMyTurn() const;
		TBSGameState2 getGameState() const;

	private:
		AbstractTBSGame* game;
		std::unique_ptr<Agent> agent;
		std::mt19937 rngEngine;
		std::thread thread;
	};

}
