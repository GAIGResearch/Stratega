#include <Stratega/ForwardModel/FunctionParameter.h>
#include <Stratega/ForwardModel/ExpressionStruct.h>
#include <Stratega/Representation/GameState.h>
#include <boost/random.hpp>
#include <Stratega/Utils/cparse/shunting-yard.h>

#include <Stratega/Utils/cparse/shunting-yard.h>

namespace SGA
{
	
	FunctionParameter::Data::Data()
	{
		//not used
	}

	FunctionParameter::Data::Data(double data)
	{
		constValue = data;
	}
	FunctionParameter::Data::Data(ExpressionStruct newExpression)
	{
		new (&expression) auto(newExpression);
	}
	FunctionParameter::Data::Data(DiceAnotation newDiceAnotation)
	{
		diceAnotation = newDiceAnotation;
	}
	FunctionParameter::Data::Data(ParameterReference data)
	{
		parameterData = data;
	}
	FunctionParameter::Data::Data(ContinuousActionReference data)
	{
		continuousActionData = data;
	}
	FunctionParameter::Data::Data(Type type, int data)
	{
		switch (type)
		{
		case Type::ArgumentReference: argumentIndex = static_cast<size_t>(data); break;
		case Type::EntityTypeReference: entityTypeID = data; break;
		case Type::ActionTypeReference: actionTypeID = data; break;
		case Type::TileTypeReference: tileTypeID = data; break;
		case Type::TechnologyTypeReference: technologyTypeID = data; break;
		case Type::BuffTypeReference:   buffTypeID = data; break;
		case Type::TimeReference:   argumentIndex = static_cast<size_t>(data); break;
		case Type::EntityPlayerReference:   argumentIndex = static_cast<size_t>(data); break;
		default:
			throw std::runtime_error("Unknown Type");
		}
	}

	FunctionParameter::Data::~Data()
	{
		//expression.~ExpressionStruct();
	}

	// Private since this class should only be constructed using the static methods
	//FunctionParameter(const Type& newType, const boost::variant<double, int, ContinuousActionReference, size_t, ParameterReference, DiceAnotation, ExpressionStruct>& newData) : parameterType(newType), data(newData) {};
	FunctionParameter::FunctionParameter(const Type& newType, const Data& newData) : parameterType(newType)/*, data(newData)*/
	{
		switch (parameterType)
		{
		case Type::Constant: data.constValue = newData.constValue; break;
		case Type::EntityPlayerReference: data.argumentIndex = newData.argumentIndex; break;
		case Type::EntityPlayerParameterReference: data.parameterData = newData.parameterData; break;
		case Type::Expression:  new (&data.expression) auto(newData.expression); break;
		case Type::ParameterReference: data.parameterData = newData.parameterData; break;
		case Type::GameStateParameterReference: data.parameterData = newData.parameterData; break;
		case Type::TileParameterReference: data.parameterData = newData.parameterData; break;
		case Type::ArgumentReference: data.argumentIndex = newData.argumentIndex; break;
		case Type::EntityTypeReference: data.entityTypeID = newData.entityTypeID; break;
		case Type::ActionTypeReference: data.actionTypeID = newData.actionTypeID; break;
		case Type::TileTypeReference: data.tileTypeID = newData.tileTypeID; break;
		case Type::TechnologyTypeReference: data.technologyTypeID = newData.technologyTypeID; break;
		case Type::BuffTypeReference: data.buffTypeID = newData.buffTypeID; break;
		case Type::TimeReference: data.argumentIndex = newData.argumentIndex; break;
		case Type::DiceAnotation: data.diceAnotation = newData.diceAnotation; break;
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	};

	FunctionParameter::FunctionParameter(const FunctionParameter& other)
	{
		parameterType = other.parameterType;
		switch (parameterType)
		{
		case Type::Constant: data.constValue = other.data.constValue; break;
		case Type::EntityPlayerReference: data.argumentIndex = other.data.argumentIndex; break;
		case Type::EntityPlayerParameterReference: data.parameterData = other.data.parameterData; break;
		case Type::Expression:  new (&data.expression) auto(other.data.expression); break;
		case Type::ParameterReference: data.parameterData = other.data.parameterData; break;
		case Type::GameStateParameterReference: data.parameterData = other.data.parameterData; break;
		case Type::TileParameterReference: data.parameterData = other.data.parameterData; break;
		case Type::ArgumentReference: data.argumentIndex = other.data.argumentIndex; break;
		case Type::EntityTypeReference: data.entityTypeID = other.data.entityTypeID; break;
		case Type::ActionTypeReference: data.actionTypeID = other.data.actionTypeID; break;
		case Type::TileTypeReference: data.tileTypeID = other.data.tileTypeID; break;
		case Type::TechnologyTypeReference: data.technologyTypeID = other.data.technologyTypeID; break;
		case Type::BuffTypeReference: data.buffTypeID = other.data.buffTypeID; break;
		case Type::TimeReference: data.argumentIndex = other.data.argumentIndex; break;
		case Type::DiceAnotation: data.diceAnotation = other.data.diceAnotation; break;
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}
	FunctionParameter::~FunctionParameter()
	{

	}

	void ExpressionStruct::addParameter(FunctionParameter parameter, std::string newVariable)
	{
		variable.insert(std::make_pair(newVariable, std::make_shared<FunctionParameter>(parameter)));
	}

	void ExpressionStruct::setExpression(std::string string)
	{
		expression = string;
	}

	std::string ExpressionStruct::getExpression(const GameState& state, const std::vector<ActionTarget>& actionTargets)
	{
		std::string ex = expression;
		for (auto& var : variable)
		{
			if (var.second->getType() == FunctionParameter::Type::Constant || var.second->getType() == FunctionParameter::Type::ParameterReference)
			{
				//Parse consant
				double temp = var.second->getConstant(state, actionTargets);
				ex = std::regex_replace(ex, std::regex(var.first), std::to_string(temp));
			}
		}
		return ex;
	}

	std::unordered_map<ParameterID, std::string> ExpressionStruct::getExpressionCost(const GameState& state, const std::vector<ActionTarget>& actionTargets)
	{
		std::string ex = expression;
		std::unordered_map<ParameterID, std::string> expressions;

		//Gett all the const expressions
		for (auto& var : variable)
		{
			if (var.second->getType() == FunctionParameter::Type::ArgumentReference)
			{
				auto& actionTarget = var.second->getActionTarget(actionTargets);
				//auto& actionTarget = actionTargets[var.second.data.argumentIndex];

				if (actionTarget.getType() == ActionTarget::TechnologyReference)
				{
					//Parse cost
					const auto& originalCost = var.second->getTechnology(state, actionTargets).cost;
					for (const auto& parameter : originalCost)
					{
						std::string ex = expression;
						ex = std::regex_replace(ex, std::regex(var.first), std::to_string(parameter.second));
						expressions[parameter.first] = ex;
					}
				}
				else if (actionTarget.getType() == ActionTarget::EntityTypeReference)
				{
					//Parse cost
					const auto& originalCost = var.second->getEntityType(state, actionTargets).getCosts();
					for (const auto& parameter : originalCost)
					{
						std::string ex = expression;
						ex = std::regex_replace(ex, std::regex(var.first), std::to_string(parameter.second));
						expressions[parameter.first] = ex;
					}
				}
				else
				{
					throw std::runtime_error("Unknow action target in expression");
				}
			}
		}

		//Check if we have constants
		for (auto& var : variable)
		{
			for (auto& currExpression : expressions)
			{
				if (var.second->getType() == FunctionParameter::Type::Constant || var.second->getType() == FunctionParameter::Type::ParameterReference)
				{
					//Parse consant
					double temp = var.second->getConstant(state, actionTargets);
					currExpression.second = std::regex_replace(currExpression.second, std::regex(var.first), std::to_string(temp));
				}
			}
		}

		return expressions;
	}

	FunctionParameter FunctionParameter::createExpression(ExpressionStruct expression)
	{
		return FunctionParameter(Type::Expression, { expression });
	}

	FunctionParameter FunctionParameter::createConstParameter(double constValue)
	{
		return FunctionParameter(Type::Constant, { constValue });
	}

	FunctionParameter FunctionParameter::createDiceAnotation(DiceAnotation dic)
	{
		return FunctionParameter(Type::DiceAnotation, { dic });
	}

	FunctionParameter FunctionParameter::createArgumentReference(int argumentIndex)
	{
		return FunctionParameter(Type::ArgumentReference, { Type::ArgumentReference, argumentIndex });
	}

	FunctionParameter FunctionParameter::createParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::ParameterReference, { ref });
	}

	FunctionParameter FunctionParameter::createEntityPlayerReference(int argumentIndex)
	{
		return FunctionParameter(Type::EntityPlayerReference, { Type::EntityPlayerReference, argumentIndex });
	}

	FunctionParameter FunctionParameter::createTimeReference(int argumentIndex)
	{
		return FunctionParameter(Type::TimeReference, { Type::TimeReference, argumentIndex });
	}

	FunctionParameter FunctionParameter::createEntityPlayerParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::EntityPlayerParameterReference, { ref });
	}

	FunctionParameter FunctionParameter::createGameStateParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::GameStateParameterReference, { ref });
	}

	FunctionParameter FunctionParameter::createTileParameterReference(ParameterReference ref)
	{
		return FunctionParameter(Type::TileParameterReference, { ref });
	}

	FunctionParameter FunctionParameter::createEntityTypeReference(int entityTypeID)
	{
		return FunctionParameter(Type::EntityTypeReference, { Type::EntityTypeReference, entityTypeID });
	}

	FunctionParameter FunctionParameter::createActionTypeReference(int actionTypeID)
	{
		return FunctionParameter(Type::ActionTypeReference, { Type::ActionTypeReference, actionTypeID });
	}

	FunctionParameter FunctionParameter::createTileTypeReference(int tileTypeID)
	{
		return FunctionParameter(Type::TileTypeReference, { Type::TileTypeReference, tileTypeID });
	}

	FunctionParameter FunctionParameter::createTechnologyTypeReference(int technologyTypeID)
	{
		return FunctionParameter(Type::TechnologyTypeReference, { Type::TechnologyTypeReference, technologyTypeID });
	}
	FunctionParameter FunctionParameter::createBuffTypeReference(int buffTypeID)
	{
		return FunctionParameter(Type::BuffTypeReference, { Type::BuffTypeReference, buffTypeID });
	}

	FunctionParameter::Type FunctionParameter::getType() const
	{
		return parameterType;
	}

	// ToDo Remove this
	const ActionTarget& FunctionParameter::getActionTarget(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ArgumentReference)
		{
			return actionTargets[data.argumentIndex];
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	double FunctionParameter::getConstant(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::Constant: return data.constValue;
		case Type::Expression:
		{
			ExpressionStruct test = data.expression;
			std::string expres = test.getExpression(state, actionTargets);				
			double value = cparse::calculator::calculate(expres.c_str()).asDouble();
			//std::cout << "Calculate expression: " << expres << " -> " << value << std::endl;
			return value;
		}			
		case Type::DiceAnotation: 
		{
			DiceAnotation test = data.diceAnotation;
			auto rndEngine = state.getRndEngineCopy();
			rndEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
			boost::uniform_int<int> distribution(1, test.diceFaceNumber);
			double result=0;
	
			for (size_t i = 0; i < test.diceNumber; i++)
			{
				result += distribution(rndEngine);
			}
	
			//std::cout << "Roll dice: " << test.diceNumber<<"d"<<test.diceFaceNumber << " -> " << result<<std::endl;
			return result;
		}
		case Type::ParameterReference:
		case Type::EntityPlayerParameterReference: return getParameterValue(state, actionTargets);
		case Type::TimeReference: return getTime(state, actionTargets);
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	double FunctionParameter::getTime(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::TimeReference)
		{
			auto& target = actionTargets[data.argumentIndex];
			if (target.getType() == ActionTarget::TechnologyReference)
			{
				auto technologyID = target.getTechnologyID();
				return state.getGameInfo()->getTechnologyTreeCollection().getTechnology(technologyID).continuousActionTime;
			}
			else if (target.getType() == ActionTarget::EntityReference
				|| target.getType() == ActionTarget::EntityTypeReference)
			{
				auto& entityType = target.getEntityType(state);
				return entityType.getContinuousActionTime();
			}

		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const Parameter& FunctionParameter::getParameter(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference
				|| actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Object)
			{
				auto& entity = getEntity(state, actionTargets);

				const auto& entityType = entity.getEntityType();
				const auto& param = entityType.getParameter(data.parameterData.parameterID);
				return param;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::TileReference)
			{
				auto& tile = getTile(state, actionTargets);

				const auto& tileType = tile.getTileType();
				const auto& param = tileType.getParameter(data.parameterData.parameterID);
				return param;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				const auto& param = state.getGameInfo()->getPlayerParameter(data.parameterData.parameterID);

				return param;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				const auto& param = state.getGameInfo()->getStateParameter(data.parameterData.parameterID);

				return param;
			}

		}
		if (parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = state.getGameInfo()->getPlayerParameterTypes().at(data.parameterData.parameterID);
			return param;
		}
		if (parameterType == Type::GameStateParameterReference)
		{
			const auto& param = state.getGameInfo()->getStateParameterTypes().at(data.parameterData.parameterID);
			return param;
		}


		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	bool FunctionParameter::isPlayerParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				return false;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				return true;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return false;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::TileReference)
			{
				return false;
			}

		}
		if (parameterType == Type::EntityPlayerParameterReference)
		{
			return true;
		}
		if (parameterType == Type::GameStateParameterReference)
		{
			return false;
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	bool FunctionParameter::isEntityParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				return true;
			}
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Object)
			{
				return true;
			}
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::SlotObject)
			{
				return true;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				return false;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::TileReference)
			{
				return false;
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return false;
			}

		}
		if (parameterType == Type::EntityPlayerParameterReference || parameterType == Type::GameStateParameterReference)
		{
			return false;
		}


		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	
	bool FunctionParameter::isPlayerReference(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ArgumentReference)
		{
			if (actionTargets[data.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				return true;
			}
			else
				return false;
		}
		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	bool FunctionParameter::isEntityReference(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ArgumentReference)
		{
			if (actionTargets[data.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				return true;
			}
			else
				return false;
		}
		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	bool FunctionParameter::isStateParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return true;
			}
			return false;
		}
		if (parameterType == Type::GameStateParameterReference)
		{
			return true;
		}
		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	
	bool FunctionParameter::isTileParameter(const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::TileReference)
			{
				return true;
			}
			return false;
		}
		if (parameterType == Type::TileParameterReference)
		{
			return true;
		}
		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	double FunctionParameter::getParameterValue(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getRawParameterValue(const_cast<GameState&>(state), actionTargets);
		/*if(parameterType == Type::ParameterReference)
		{
			const auto& param = getParameter(const_cast<GameState&>(state), actionTargets);
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference)
			{
				const auto& entity = getEntity(state, actionTargets);
				return entity.getParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				auto& player = getPlayer(state, actionTargets);
				return player.getParameter(param.getIndex());
			}
		}
		if(parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = getParameter(const_cast<GameState&>(state), actionTargets);
			auto& entity = getEntity(state, actionTargets);
			auto* player = state.getPlayer(entity.getOwnerID());
			return player->getParameter(param.getIndex());
		}
		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");*/
	}

	double FunctionParameter::getParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getRawParameterValue(const_cast<GameState&>(state), actionTargets);
	}

	double& FunctionParameter::getRawParameterValue(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::EntityReference ||
				actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Object)
			{
				//std::cout <<"		" << param.getName()<<" "<<std::endl;
				auto& entity = getEntity(state, actionTargets);
				return entity.getRawParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::PlayerReference)
			{
				auto& player = getPlayer(state, actionTargets);
				return player.getRawParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::TileReference)
			{
				auto& tile = getTile(state, actionTargets);
				return tile.getRawParameterAt(param.getIndex());
			}
			else if (actionTargets[data.parameterData.argumentIndex].getType() == ActionTarget::Gamestate)
			{
				return state.getRawParameterAt(param.getIndex());
			}
		}
		if (parameterType == Type::EntityPlayerParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			auto& entity = getEntity(state, actionTargets);
			auto* player = state.getPlayer(entity.getOwnerID());
			return player->getRawParameterAt(param.getIndex());
		}
		if (parameterType == Type::GameStateParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			return state.getRawParameterAt(param.getIndex());
		}
		if (parameterType == Type::TileParameterReference)
		{
			const auto& param = getParameter(state, actionTargets);
			return state.getRawParameterAt(param.getIndex());
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	Vector2f FunctionParameter::getPosition(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ArgumentReference)
		{
			return actionTargets[data.argumentIndex].getPosition(state);
		}
		else
		{
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	Entity& FunctionParameter::getEntity(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerReference:
		case Type::ArgumentReference:
		{
			auto entityID = actionTargets[data.argumentIndex].getEntityID();
			return *state.getEntity(entityID);
		}
		case Type::ParameterReference:
		case Type::EntityPlayerParameterReference:
		{
			auto entityID = actionTargets[data.parameterData.argumentIndex].getEntityID();
			return *state.getEntity(entityID);
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	Tile& FunctionParameter::getTile(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::ArgumentReference:
		{
			auto pos = actionTargets[data.argumentIndex].getPosition();
			return state.getTileAt({ static_cast<int>(pos.x), static_cast<int>(pos.y)});
		}
		case Type::ParameterReference:
		{
			auto pos = actionTargets[data.parameterData.argumentIndex].getPosition();
			return state.getTileAt({ static_cast<int>(pos.x), static_cast<int>(pos.y) });
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const Tile& FunctionParameter::getTile(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::ArgumentReference:
		{
			auto pos = actionTargets[data.argumentIndex].getPosition();
			return state.getTileAtConst({ static_cast<int>(pos.x), static_cast<int>(pos.y)});
		}
		case Type::ParameterReference:
		{
			auto pos = actionTargets[data.parameterData.argumentIndex].getPosition();
			return state.getTileAtConst({ static_cast<int>(pos.x), static_cast<int>(pos.y)});
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	Entity& FunctionParameter::getObject(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerReference:
		case Type::ArgumentReference:
		{
			auto entityID = actionTargets[data.argumentIndex].getObjectID();
			return *state.getObject(entityID);
		}
		case Type::ParameterReference:
		case Type::EntityPlayerParameterReference:
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}


	Entity& FunctionParameter::getSlotObject(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerReference:
		case Type::ArgumentReference:
		{
			auto entityID = actionTargets[data.argumentIndex].getObjectID();
			return *state.getSlotObject(entityID);
		}
		case Type::ParameterReference:
		case Type::EntityPlayerParameterReference:
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const Entity& FunctionParameter::getObject(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerReference:
		case Type::ArgumentReference:
		{
			auto entityID = actionTargets[data.argumentIndex].getObjectID();
			return *state.getObjectConst(entityID);
		}
		case Type::ParameterReference:
		case Type::EntityPlayerParameterReference:
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}


	const Entity& FunctionParameter::getSlotObject(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerReference:
		case Type::ArgumentReference:
		{
			auto entityID = actionTargets[data.argumentIndex].getObjectID();
			return *state.getSlotObjectConst(entityID);
		}
		case Type::ParameterReference:
		case Type::EntityPlayerParameterReference:
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const Entity& FunctionParameter::getEntity(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getEntity(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}

	Player& FunctionParameter::getPlayer(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{

		case Type::ParameterReference:
		{
			return actionTargets[data.parameterData.argumentIndex].getPlayer(state);
		}
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			auto& entity = getEntity(state, actionTargets);
			return *state.getPlayer(entity.getOwnerID());
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getPlayer(state);
		}
		case Type::Constant:
		{
			return *state.getPlayer(static_cast<int>(data.constValue));
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}


	int FunctionParameter::getPlayerID(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			const auto& entity = getEntity(state, actionTargets);
			return entity.getOwnerID();
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getPlayerID(state);
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const Player& FunctionParameter::getPlayer(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		return getPlayer(const_cast<GameState&>(state), const_cast<std::vector<ActionTarget>&>(actionTargets));
	}

	const EntityType& FunctionParameter::getEntityType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::EntityTypeReference)
		{
			return state.getGameInfo()->getEntityType(data.entityTypeID);
		}
		if (parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return actionTarget.getEntityType(state);
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const ActionType& FunctionParameter::getActionType(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ActionTypeReference)
		{
			return state.getGameInfo()->getActionType(data.actionTypeID);
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
	const BuffType& FunctionParameter::getBuffType(const GameState& state, const std::vector<ActionTarget>& /*actionTargets*/) const
	{
		if (parameterType == Type::BuffTypeReference)
		{
			return state.getGameInfo()->getBuffType(data.buffTypeID);
		}
		/*if(parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return actionTarget.getEntityType(state);
		}*/

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const std::unordered_set<EntityTypeID>& FunctionParameter::getSpawnableEntities(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		switch (parameterType)
		{
		case Type::ParameterReference:
		{
			return actionTargets[data.parameterData.argumentIndex].getSpawnableEntities(state);
		}
		case Type::EntityPlayerParameterReference:
		case Type::EntityPlayerReference:
		{
			const auto& entity = getEntityType(state, actionTargets);
			return entity.getSpawnableEntityTypes();
		}
		case Type::ArgumentReference:
		{
			return actionTargets[data.argumentIndex].getSpawnableEntities(state);
		}
		default:
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}
	}

	const TechnologyTreeNode& FunctionParameter::getTechnology(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{

		if (parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			return state.getGameInfo()->getTechnologyTreeCollection().getTechnology(actionTarget.getTechnologyID());
		}
		else if (parameterType == Type::TechnologyTypeReference)
		{
			return state.getGameInfo()->getTechnologyTreeCollection().getTechnology(data.technologyTypeID);
		}
		else
		{
			throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
		}

	}

	const std::unordered_map<ParameterID, double> FunctionParameter::getCost(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (parameterType == Type::ArgumentReference)
		{
			const auto& actionTarget = actionTargets[data.argumentIndex];
			if (actionTarget.getType() == ActionTarget::EntityTypeReference)
			{
				return getEntityType(state, actionTargets).getCosts();
			}
			else if (actionTarget.getType() == ActionTarget::TechnologyReference)
			{
				return getTechnology(state, actionTargets).cost;
			}
		}
		else if (parameterType == Type::TechnologyTypeReference)
		{
			return getTechnology(state, actionTargets).cost;
		}		
		else if (parameterType == Type::EntityTypeReference)
		{
			return getEntityType(state, actionTargets).getCosts();
		}
		else if (parameterType == Type::Expression)
		{
			std::unordered_map<ParameterID, double> newCost;
			ExpressionStruct test = data.expression;
			std::unordered_map<ParameterID, std::string> expressions = test.getExpressionCost(state, actionTargets);
			for (auto& expression : expressions)
			{
				newCost[expression.first] = cparse::calculator::calculate(expression.second.c_str()).asDouble();
			}
			/*double value = cparse::calculator::calculate(expres.c_str()).asDouble();*/
			//std::cout << "Calculate expression: " << expres << " -> " << value << std::endl;

			return newCost;
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	std::unordered_map<int, double>& FunctionParameter::getParameterList(GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			auto& player = getPlayer(state, actionTargets);
			return player.getParameters();

		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				auto& player = target.getPlayer(state);
				return player.getParameters();
			}
			else if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(state);
				return sourceEntity.getParamValues();
			}
		}
		else
		{
			auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.getParamValues();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const std::unordered_map<int, double>& FunctionParameter::getParameterList(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			auto& player = getPlayer(state, actionTargets);
			return player.getParameters();

		}
		else if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				auto& player = target.getPlayer(const_cast<GameState&>(state));
				return player.getParameters();
			}
			else if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(const_cast<GameState&>(state));
				return sourceEntity.getParamValues();
			}
		}
		else
		{
			const auto& sourceEntity = getEntity(state, actionTargets);
			return sourceEntity.getParamValues();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const std::unordered_map<ParameterID, Parameter>& FunctionParameter::getParameterLookUp(const GameState& state, const std::vector<ActionTarget>& actionTargets) const
	{
		if (getType() == Type::EntityPlayerReference)
		{
			return state.getGameInfo()->getPlayerParameterTypes();

		}
		if (getType() == Type::ArgumentReference)
		{
			const auto& target = getActionTarget(actionTargets);
			if (target.getType() == ActionTarget::PlayerReference)
			{
				return state.getGameInfo()->getPlayerParameterTypes();
			}
			if (target.getType() == ActionTarget::EntityReference)
			{
				auto& sourceEntity = *target.getEntity(const_cast<GameState&>(state));
				const auto& sourceEntityType = sourceEntity.getEntityType();
				return sourceEntityType.getParameters();
			}
		}
		else
		{
			const auto& sourceEntity = getEntity(state, actionTargets);
			const auto& sourceEntityType = sourceEntity.getEntityType();
			return sourceEntityType.getParameters();
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}

	const TileType& FunctionParameter::getTileType(const GameState& state, const std::vector<ActionTarget>& /*actionTargets*/) const
	{
		if (parameterType == Type::TileTypeReference)
		{
			return state.getGameInfo()->getTileType(data.tileTypeID);
		}

		throw std::runtime_error("Parameter type " + std::to_string(int(parameterType)) + " not recognised in function parameter.");
	}
}