#pragma once
#include <ForwardModel/ConditionType.h>
#include <ForwardModel/EffectType.h>

namespace SGA
{
	struct EffectOld
	{
		// Condition variables
		ConditionType conditionType;
		int targetTileTypeID;
		
		// Effect variables
		EffectType type;
		int damage;
	};
}
