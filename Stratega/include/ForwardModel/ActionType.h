#pragma once
#include <string>

namespace SGA
{
	enum class ActionType
	{
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
		case ActionType::Attack: return "Attack"; break;
		case ActionType::Move: return "Move"; break;
		case ActionType::Heal: return "Heal"; break;
		case ActionType::Push: return "Push"; break;
		case ActionType::EndTurn: return "EndTurn"; break;
		default: return "Not found"; break;
		}
	}
}