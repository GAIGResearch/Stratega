#pragma once
#include <Game/GameCommunicator.h>
#include <Game/RTSGame.h>
#include <random>
#include <thread>


namespace SGA
{
	class RTSAction
	{
		std::unordered_map<int, Action> entitiesAction;
		std::vector<Action> playerAction;

		//Add new action to the entities map or player list
		void assignAction(Action newAction);

		//Mix two rtsaction
		void accumulate(RTSAction action);
	};
	
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
		bool isGameOver() const;

		//TODO private
		void setGame(RTSGame& newGame);
		void setAgent(std::unique_ptr<Agent> agent);
		void setRNGEngine(std::mt19937 engine) { rngEngine = engine; }

		void executeAction(Action action) const;
		void executeAction(RTSAction action) const;
		bool isMyTurn() const;
		RTSGameState getGameState() const;

	private:
		RTSGame* game;
		std::unique_ptr<Agent> agent;
		std::mt19937 rngEngine;
		std::thread thread;
	};
}
