#pragma once
#include <Game/GameCommunicator.h>
#include <Representation/RTSGameState.h>
#include <Game/RTSGame.h>
#include <Agent/Agent.h>

#include <random>
#include <thread>

namespace SGA
{
	class Agent;
	
	class RTSGameCommunicator : public GameCommunicator
	{
	public:
		RTSGameCommunicator(int playerID) :
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
		bool RTSGameCommunicator::isGameOver() const;

		//TODO private
		void setGame(RTSGame& newGame);
		void setAgent(std::unique_ptr<Agent> agent);
		void setRNGEngine(std::mt19937 engine) { rngEngine = engine; }

		void executeAction(Action<Vector2f> action) const;
		RTSGameState getGameState() const;

	private:
		RTSGame* game;
		std::unique_ptr<Agent> agent;
		std::mt19937 rngEngine;
		std::thread thread;
	};
}
