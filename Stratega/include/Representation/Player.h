#pragma once
#include <Representation/TBSUnit.h>
#include <Representation/RTSUnit.h>
#include <Representation/TBSGameState.h>
#include <Representation/RTSGameState.h>
namespace SGA
{
	class TBSGameState;
	class TBSUnit;
	class RTSGameState;
	class RTSUnit;
	
	class TBSPlayer
	{
	public:
		TBSPlayer(TBSGameState& state, int playerID);
		
		std::reference_wrapper<TBSGameState> state;
		int playerID;
		int score;
		bool canPlay = true;

		std::vector<TBSUnit*> getUnits() const;
	};

	class RTSPlayer
	{
	public:
		RTSPlayer(RTSGameState& state, int playerID);

		std::reference_wrapper<RTSGameState> state;
		int playerID;
		int score;
		bool canPlay = true;

		std::vector<RTSUnit*> getUnits() const;
	private:
	};
}