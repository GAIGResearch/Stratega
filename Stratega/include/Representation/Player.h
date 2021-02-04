#pragma once
#include <Representation/Parameter.h>
#include <ForwardModel/Action.h>
#include <unordered_map>
#include <vector>

namespace SGA
{
	struct Player
	{
		static const int NEUTRAL_PLAYER_ID = -1;
		
		int id;
		double score;
		bool canPlay;
		std::vector<double> parameters;
		std::vector<Action> continuousAction;

		std::vector<ActionInfo> attachedActions;
		const Parameter& getParameter(ParameterID id) const;
	};
}
