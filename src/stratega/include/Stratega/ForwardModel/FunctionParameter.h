#pragma once
#include <vector>
#include <Stratega/ForwardModel/ActionTarget.h>
#include <Stratega/Representation/EntityType.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/TechnologyTree.h>
#include <Stratega/Representation/TileType.h>
#include <Stratega/Representation/BuffType.h>
#include <boost/variant.hpp>

namespace SGA
{
	struct GameState;
	
	class FunctionParameter
	{
	public:
		enum class Type
		{
			Constant, // Values like 1, 1.5, etc
			ParameterReference, // References the parameter of an action-target. Source.Gold references to the gold of the source of an action
			EntityPlayerReference, // References the player indirectly. Like Source.Player, only works if Source is a entity
			EntityPlayerParameterReference, // References the parameter of an player indirectly. Like Source.Player.Gold, only works if Source is a entity
			ArgumentReference, // References an action-argument, like Source or Target. Can be used for example if you want to pass a position-target
			EntityTypeReference, // References EntityTypes defined in the Game. Like Warrior, GoldMine, etc.
			TechnologyTypeReference, // References TechnologyTypes defined in the Game,
			TimeReference, // References the Time for specific Entities or Technologies,
			TileTypeReference, // References TileTypes defined in the Game. Like Plain, Forest...
			BuffTypeReference, // References BuffTypes defined in the Game. Like Plain, Forest...
			GameStateParameterReference, // References BuffTypes defined in the Game. Like Plain, Forest...
			Expression, //List of diferent string + function parameters (constant + parameter reference)
			DiceAnotation // 4D12
		};

		struct ExpressionStruct
		{
		private:
			std::vector<boost::variant<FunctionParameter, std::string>> dataVector;

		public:
			void addParameter(FunctionParameter parameter)
			{
				dataVector.emplace_back(parameter);
			}

			void addString(std::string string)
			{
				dataVector.emplace_back(string);
			}

			std::string getExpression(const GameState& state, const std::vector<ActionTarget>& actionTargets)
			{
				std::string expression = "";
				for (auto& point : dataVector)
				{
					if (point.type() == typeid(FunctionParameter))
					{
						auto pr = boost::get<FunctionParameter>(point);
						expression += std::to_string(pr.getConstant(state, actionTargets));
					}
					else
					{
						expression += boost::get<std::string>(point);
					}
				}

				return expression;
			}
		};

	private:
		

		struct ParameterReference
		{
			ParameterID parameterID;
			size_t argumentIndex;

			ParameterReference(ParameterID newParameterID, int newArgumentIndex):
				parameterID(newParameterID),
				argumentIndex(static_cast<size_t>(newArgumentIndex))
			{
			}
		};

		struct ContinuousActionReference
		{
			int sourceEntity;
			int actionID;

			ContinuousActionReference(int newSourceEntity, int newActionID) :
				sourceEntity(newSourceEntity),
				actionID(newActionID)
			{
			}
		};

		


		boost::variant<double, int, ContinuousActionReference, size_t, ParameterReference, ExpressionStruct> data;
		//union Data
		//{
		//	Data(double data)
		//	{
		//		constValue = data;
		//	}
		//	/*Data(std::string newExpression)
		//	{
		//		expression = newExpression;
		//	}*/
		//	Data(ParameterReference data)
		//	{
		//		parameterData = data;
		//	}
		//	Data(ContinuousActionReference data)
		//	{
		//		continuousActionData = data;
		//	}
		//	Data (Type type, int data)
		//	{
		//		switch (type)
		//		{
		//		case Type::ArgumentReference: argumentIndex = static_cast<size_t>(data); break;
		//		case Type::EntityTypeReference: entityTypeID = data; break;
		//		case Type::TileTypeReference: tileTypeID = data; break;
		//		case Type::TechnologyTypeReference: technologyTypeID = data; break;
		//		case Type::BuffTypeReference:   buffTypeID = data; break;
		//		case Type::TimeReference:   argumentIndex = static_cast<size_t>(data); break;
		//		case Type::EntityPlayerReference:   argumentIndex = static_cast<size_t>(data); break;
		//		default:
		//			throw std::runtime_error("Unknown Type");
		//		}
		//	}
		//	double constValue;
		//	ParameterReference parameterData;
		//	size_t argumentIndex;
		//	int entityTypeID;
		//	int tileTypeID;
		//	int technologyTypeID;
		//	int buffTypeID;
		//	ContinuousActionReference continuousActionData;
		//	//std::string expression;
		//};
		
		Type parameterType;
		//Data data;

		// Private since this class should only be constructed using the static methods
		FunctionParameter(const Type& newType, const boost::variant<double, int, ContinuousActionReference, size_t, ParameterReference, ExpressionStruct>& newData) : parameterType(newType), data(newData) {};

	public:
		static FunctionParameter createConstParameter(double constValue);
		static FunctionParameter createArgumentReference(int argumentIndex);
		static FunctionParameter createParameterReference(ParameterReference ref);
		static FunctionParameter createEntityPlayerReference(int argumentIndex);
		static FunctionParameter createTimeReference(int argumentIndex);
		static FunctionParameter createEntityPlayerParameterReference(ParameterReference ref);
		static FunctionParameter createGameStateParameterReference(ParameterReference ref);
		static FunctionParameter createEntityTypeReference(int entityTypeID);
		static FunctionParameter createTileTypeReference(int tileTypeID);
		static FunctionParameter createTechnologyTypeReference(int technologyTypeID);
		static FunctionParameter createBuffTypeReference(int buffTypeID);
		static FunctionParameter createExpression(ExpressionStruct expression);

		Type getType() const;
		const ActionTarget& getActionTarget(const std::vector<ActionTarget>& actionTargets) const;
		
		/*std::string getExpression() const {
			return boost::get<std::string>(data);
		}*/
		double getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Parameter& getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double getParameterValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double getParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double& getRawParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Vector2f getPosition(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Entity& getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Entity& getObject(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Entity& getSlotObject(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Entity& getObject(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Entity& getSlotObject(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Entity& getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		Player& getPlayer(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		int getPlayerID(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const std::unordered_set<EntityTypeID>& getSpawnableEntities(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const Player& getPlayer(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const EntityType& getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const BuffType& getBuffType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		
		const TechnologyTreeNode& getTechnology(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const std::unordered_map<ParameterID, double>& getCost(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const std::unordered_map<ParameterID, Parameter>& getParameterLookUp(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		double getTime(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		std::vector<double>& getParameterList(GameState& state, const std::vector<ActionTarget>& actionTargets) const;
		const std::vector<double>& getParameterList(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;

		const TileType& getTileType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const;

		bool isPlayerParameter(const std::vector<ActionTarget>& actionTargets) const;
		bool isEntityParameter(const std::vector<ActionTarget>& actionTargets) const;
		bool isStateParameter(const std::vector<ActionTarget>& actionTargets) const;
	};
}
