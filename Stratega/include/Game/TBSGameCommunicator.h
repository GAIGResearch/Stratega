#pragma once
#include <Game/GameCommunicator.h>
#include <Representation/TBSGameState.h>
#include <Game/TBSGame.h>

#include <random>
#include <thread>

namespace SGA
{
	class Agent;

	class TBSGameCommunicator : public GameCommunicator
	{
	public:
		TBSGameCommunicator(int playerID) :
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
		void setGame(TBSGame& newGame);
		void setAgent(std::unique_ptr<Agent> agent);
		void setRNGEngine(std::mt19937 engine) { rngEngine = engine; }

		void executeAction(Action action) const;
		bool isMyTurn() const;
		TBSGameState getGameState() const;

	private:
		TBSGame* game;
		std::unique_ptr<Agent> agent;
		std::mt19937 rngEngine;
		std::thread thread;
	};

}
