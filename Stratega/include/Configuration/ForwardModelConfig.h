#pragma once
#include <iostream>
#include <map>
#include <Configuration/WinConditionType.h>
#include <Configuration/EffectConfig.h>

namespace SGA
{
	struct ForwardModelConfig
	{
		WinConditionType WinCondition;
		//Only for UnitAlive condition
        std::string unitType;
		
        std::map<std::string, EffectConfig> effects;

        ForwardModelConfig():
			WinCondition(WinConditionType::LastManStanding)
		{
			
		}
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::ForwardModelConfig>
    {
        static bool decode(const Node& node, SGA::ForwardModelConfig& rhs)
        {
            Node nodeWincondition = node["WinCondition"];
            rhs.WinCondition = nodeWincondition["Type"].as<SGA::WinConditionType>(rhs.WinCondition);
        	
            rhs.unitType= nodeWincondition["Unit"].as<std::string>(rhs.unitType);

            rhs.effects = node["Effects"].as<std::map<std::string, SGA::EffectConfig>>(rhs.effects);
            return true;
        }
    };
}
