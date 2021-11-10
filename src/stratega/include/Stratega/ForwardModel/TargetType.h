#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <Stratega/ForwardModel/SamplingMethod.h>
#include <stdexcept>
namespace SGA
{
	class ActionTarget;
	struct GameState;
	
	enum class ShapeType
	{
		Circle,
		Square,
		Cross,
		Line,
		AllPositions
	};

	struct TargetType
	{
		enum Type { None, Position, EntityType, Entity, Technology, ContinuousAction};
	


	private:

		/// <summary>
		/// Type of this target, defined in enum Type {};
		/// </summary>
		Type type = Type::None;

		/// <summary>
		/// Pointer to the sampling method for this target type.
		/// </summary>
		std::shared_ptr<SamplingMethod> samplingMethod;

		/// <summary>
		/// Set of entity type IDs for this target
		/// Specific variables for targetType=Unit
		/// </summary>
		std::unordered_set<int> groupEntityTypes;

		/// <summary>
		/// Set of technology type IDs for this target
		/// Specific variables for targetType=Technology
		/// </summary>
		std::unordered_set<int> technologyTypes;

	public:

		/// <summary>
		/// Operator for type comparison
		/// </summary>
		operator Type() const { return type; }

		/// <summary>
		/// Returns a string that represents this target.
		/// </summary>
		const std::string getTypeString() const
		{
			switch (type)
			{
			case None:
				return "None";
				break;
			case Position:
				return "Position";
			break;
			case EntityType:
				return "EntityType";
				break;
			case Entity:
				return "Entity";
				break;
			case Technology:
				return "Technology";
				break;
			case ContinuousAction:
				return "ContinuousAction";
				break;
			default:
				throw std::runtime_error("Not defined");
			}
		}

		//Check if is a valid target
		bool isValid(const GameState& state, const ActionTarget& actionTarget, const ActionTarget& sourceActionTarget) const;

		/* Getters and setters for this target type */

		const Type& getType() const { return type; }
		void setType(const Type& newType) { this->type = newType; }

		std::shared_ptr<SamplingMethod> getSamplingMethod() const { return samplingMethod; }
		void setSamplingMethod(std::shared_ptr<SamplingMethod> newSamplingMethod) { this->samplingMethod = newSamplingMethod; }

		const std::unordered_set<int>& getGroupEntityTypes() const { return groupEntityTypes; }
		void setGroupEntityTypes(const std::unordered_set<int>& groupTypes) { groupEntityTypes = groupTypes; }

		const std::unordered_set<int>& getTechnologyTypes() const { return technologyTypes; }
		std::unordered_set<int>& getTechnologyTypes() { return technologyTypes; }

	};
}
