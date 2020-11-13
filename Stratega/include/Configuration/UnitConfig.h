#pragma once
#include <unordered_set>
#include <yaml-cpp/node/node.h>
#include <Representation/UnitType.h>

namespace SGA
{
	struct UnitConfig
	{
        int health;
        int attackDamage;
        int healAmount;
        int movementRange;
        int actionRange;
        int actionsPerTurn;
        bool canRepeatActions;
        int lineOfSightRange;
        float collisionRadius;
        std::unordered_set<TBSActionType> tbsActions;
		
        UnitConfig():
	        health(0),
	        attackDamage(0),
	        healAmount(0),
	        movementRange(0),
			actionRange(0),
			actionsPerTurn(1),
			canRepeatActions(false),
	        lineOfSightRange(0),
			collisionRadius(0.5)
        {
        }
	};

    UnitType unitTypeFromConfig(const UnitConfig& config, int id);
}

namespace YAML
{
    template<>
    struct convert<SGA::TBSActionType>
    {
        static bool decode(const Node& node, SGA::TBSActionType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
            if (type == "Move")
            {
                rhs = SGA::TBSActionType::Move;
            }
            else if (type == "Attack")
            {
                rhs = SGA::TBSActionType::Attack;
            }
            else if (type == "Heal")
            {
                rhs = SGA::TBSActionType::Heal;
            }
            else if (type == "Push")
            {
                rhs = SGA::TBSActionType::Push;
            }
            else
            {
                return false;
            }

            return true;
        }
    };
	
    template<>
    struct convert<SGA::UnitConfig>
    {
        static bool decode(const Node& node, SGA::UnitConfig& rhs)
        {
            if (!node.IsMap())
                return false;

            rhs.health = node["Health"].as<int>();
            rhs.lineOfSightRange = node["LineOfSightRange"].as<int>();
            rhs.actionRange = node["ActionRange"].as<int>();
            rhs.actionsPerTurn = node["ActionsPerTurn"].as<int>(rhs.actionsPerTurn);
            rhs.canRepeatActions = node["RepeatableActions"].as<bool>(rhs.canRepeatActions);
            rhs.collisionRadius = node["CollisionRadius"].as<float>(rhs.collisionRadius);
        	
            auto tbsActions = node["Actions"].as<std::vector<SGA::TBSActionType>>(std::vector<SGA::TBSActionType>());
            rhs.tbsActions.insert(tbsActions.begin(), tbsActions.end());
            rhs.tbsActions.insert(tbsActions.begin(), tbsActions.end());
        	
        	if(rhs.tbsActions.find(SGA::TBSActionType::Move) != rhs.tbsActions.end())
        	{
                rhs.movementRange = node["MovementRange"].as<int>();
        	}
        	if(rhs.tbsActions.find(SGA::TBSActionType::Attack) != rhs.tbsActions.end())
        	{
                rhs.attackDamage = node["AttackDamage"].as<int>();
        	}
        	if(rhs.tbsActions.find(SGA::TBSActionType::Heal) != rhs.tbsActions.end())
        	{
                rhs.healAmount = node["HealAmount"].as<int>();
        	}

            return true;
        }
    };
}
