#pragma once
#include <string>

namespace SGA
{
	enum class ActionType
	{
		None,
		Move,
		Attack,
		Push,
		Heal,
		EndTurn
	};
	static std::string getActionType(const ActionType& action)
	{
		switch (action)
		{
			case ActionType::None: return "None";
			case ActionType::Attack: return "Attack";
			case ActionType::Move: return "Move";
			case ActionType::Heal: return "Heal";
			case ActionType::Push: return "Push";
			case ActionType::EndTurn: return "EndTurn";
			default: return "Undefined";
		}
	}
}