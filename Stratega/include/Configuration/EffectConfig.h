#pragma once
#include <ForwardModel/EffectType.h>
#include <ForwardModel/ConditionType.h>
#include <Configuration/TriggerType.h>

namespace SGA
{
	struct EffectConfig
	{
		EffectType type;
		TriggerType trigger;
		ConditionType condition;
        std::string targetTileName;
		int amount;

        EffectConfig():
            type(EffectType::Damage),
            trigger(TriggerType::EndOfTurn),
			condition(ConditionType::None),
			targetTileName(""),
			amount(0)
		{
			
		}
	};
}

namespace YAML
{
    template<>
	struct convert<SGA::EffectConfig>
    {
	    static bool decode(const Node& node, SGA::EffectConfig& rhs)
	    {
            if (!node.IsMap())
                return false;

            rhs.type = node["Type"].as<SGA::EffectType>();
            rhs.trigger = node["Trigger"].as<SGA::TriggerType>();
            rhs.condition = node["Condition"].as<SGA::ConditionType>(rhs.condition);

	    	if(rhs.type == SGA::EffectType::Damage)
	    	{
                rhs.amount = node["Amount"].as<int>();
	    	}
            if(rhs.condition == SGA::ConditionType::StandingOnTile)
            {
                rhs.targetTileName = node["TargetTile"].as<std::string>();
            }
	    	
            return true;
	    }
    };
	
    template<>
    struct convert<SGA::ConditionType>
    {
        static bool decode(const Node& node, SGA::ConditionType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
            if (type == "StandingOnTile")
            {
                rhs = SGA::ConditionType::StandingOnTile;
            }
            else if (type == "None")
            {
                rhs = SGA::ConditionType::None;
            }
            else
            {
                return false;
            }

            return true;
        }
    };
	
    template<>
    struct convert<SGA::EffectType>
    {
        static bool decode(const Node& node, SGA::EffectType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
            if (type == "Death")
            {
                rhs = SGA::EffectType::Death;
            }
            else if (type == "Damage")
            {
                rhs = SGA::EffectType::Damage;
            }
            else
            {
                return false;
            }

            return true;
        }
    };
}