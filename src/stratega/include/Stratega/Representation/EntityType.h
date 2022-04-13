#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <unordered_set>
#include <Stratega/Representation/Parameter.h>
#include <Stratega/Representation/Entity.h>

namespace SGA
{
	typedef int EntityTypeID;
	class Effect;
	class Condition;
	/// <summary>
	/// Contains the definition of the entity type. This information includes a list of parameters definitions,
	/// the actions that can execute, required technologies to spawn a entity, a list of spawnable entity types and
	/// a list of cost assigned to this entity type.
	/// </summary>
	struct EntityType
	{

	private:
		/// <summary>
		/// Unique entity type ID
		/// </summary>
		int id;

		/// <summary>
		/// Name of this type, as defined in the YAML file.
		/// </summary>
		std::string name;

		/// <summary>
		/// Grid level, as defined in the YAML file.
		/// </summary>
		int grid=0;

		/// <summary>
		/// Symbol for this entity used in YAML for level placement.
		/// </summary>
		char symbol;

		/// <summary>
		/// ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.
		/// </summary>
		int requiredTechnologyID;

		/// <summary>
		/// Continuous Action Time
		/// </summary>
		double continuousActionTime;

		/// <summary>
		/// Range of the line of sight of this unit.
		/// </summary>
		double lineOfSightRange;

		/// <summary>
		/// Map of parameters of this entity. Key is an integer (ID of the parameter) and value is the paramter object.
		/// The parameter defines the min, max and default value this parameter can take.
		/// </summary>
		std::unordered_map<ParameterID, Parameter> parameters;

		/// <summary>
		/// List of action IDs this entity can execute.
		/// </summary>
		std::vector<int> actionIds;

		/// <summary>
		/// List of OnTickAction IDs this entity can execute.
		/// </summary>
		std::vector<int> onTickActionIds;

		/// <summary>
		/// List of entity types this entity can spwan in the game.
		/// </summary>
		std::unordered_set<EntityTypeID> spawnableEntityTypes;

		/// <summary>
		/// List of parameters and values that must be spent to spawn/build/create this unit.
		/// </summary>
		std::unordered_map<ParameterID, double> cost;
				

		//Object stuff
		//Holder
		std::vector<std::string> slots;
		std::vector<std::string> slotsUsed;
		int inventorySize;
		std::unordered_set<EntityTypeID> canEquipGroupEntityTypes;

		//Object
		std::vector<int> initialObjectsIds;

		std::vector<int> onTickObjectActionsIds;

		std::vector<std::shared_ptr<Effect>> onEquipObjectEffects;
		std::vector<std::shared_ptr<Effect>> onAddedInventoryObjectEffects;
		std::vector<std::shared_ptr<Effect>> onUseInventoryObjectEffects;
		std::vector<std::shared_ptr<Effect>> onUseSlotObjectEffects;

		std::vector<std::shared_ptr<Condition>> onEquipObjectConditions;
		std::vector<std::shared_ptr<Condition>> onAddedInventoryObjectConditions;
		std::vector<std::shared_ptr<Condition>> onUseInventoryObjectConditions;
		std::vector<std::shared_ptr<Condition>> onUseSlotObjectConditions;

	public:
		

		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameter(ParameterID id) const;
		
		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameterByIndex(int index) const;
		
		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameterByName(const std::string& name) const;

		/// <summary>
		/// Checks if this entity type is allowed to execute an action type
		/// </summary>
		bool canExecuteAction(int actionTypeID) const;

		/// <summary>
		/// Generate a new empty instance of this entity type 
		/// </summary>
		Entity instantiateEntity(int entityID) const;

		/// <summary>
		/// Returns the maximum value of a given parameter
		/// </summary>
		double getParamMax(std::string paramName) const;

		/// <summary>
		/// Returns the minimum value of a given parameter
		/// </summary>
		double getParamMin(std::string paramName) const;

		/// <summary>
		/// Get the slots Ids used by a specific ID
		/// <summary>
		std::vector<int> getSlotsUsedIds(Entity& object) const 
		{
			std::vector<int> slotsUsedIds;
			int currentId = 0;
			for (auto&currentSlot : slots)
			{
				for (auto& currentSlotUsed : object.getEntityType().getSlotsUsed())
				{
					if (currentSlot == currentSlotUsed)
					{
						slotsUsedIds.emplace_back(currentId);
						break;
					}
				}
				currentId++;
			}
			return slotsUsedIds;
		}

		/// <summary>
		/// Get the slots Id by name
		/// <summary>
		int getSlotId(std::string slotName) const
		{
			int currentId = 0;
			for (auto& currSlot : slots)
			{
				if (currSlot == slotName)
					return currentId;
				currentId++;
			}
			return -1;
		}

		/// <summary>
		/// Get the slotsIDs that are used
		/// <summary>
		std::vector<int> getSlotsUsedIds() const 
		{
			std::vector<int> slotsUsedIds;
			int currentId = 0;
			for (auto&currentSlot : slots)
			{
				for (auto& currentSlotUsed : getSlotsUsed())
				{
					if (currentSlot == currentSlotUsed)
					{
						slotsUsedIds.emplace_back(currentId);
						break;
					}
				}
				currentId++;
			}
			return slotsUsedIds;
		}

		/* Getters and setters for EntityType*/

		int getID() const { return id; }
		void setID(int newID) { this->id = newID; }

		int getGrid() const { return grid; }
		void setGrid(int newGrid) { this->grid = newGrid; }

		const std::string& getName() const { return name; }
		void setName(std::string newName) { this->name = newName; }

		char getSymbol() const { return symbol; }
		void setSymbol(char s) { symbol = s; }
		
		const std::vector<std::string>& getSlots() const { return slots; }
		void setSlots(std::vector<std::string> newSlots) { slots = newSlots; }

		const std::vector<std::shared_ptr<Effect>>& getOnEquipObjectEffects() const { return onEquipObjectEffects; }
		void setOnEquipObjectEffects(std::vector<std::shared_ptr<Effect>> newEffects) { onEquipObjectEffects = newEffects; }

		const std::vector<std::shared_ptr<Effect>>& getOnAddedInventoryObjectEffects() const { return onAddedInventoryObjectEffects; }
		void setOnAddedInventoryObjectEffects(std::vector<std::shared_ptr<Effect>> newEffects) { onAddedInventoryObjectEffects = newEffects; }

		const std::vector<std::shared_ptr<Effect>>& getOnUseInventoryObjectEffects() const { return onUseInventoryObjectEffects; }
		void setOnUseInventoryObjectEffects(std::vector<std::shared_ptr<Effect>> newEffects) { onUseInventoryObjectEffects = newEffects; }

		const std::vector<std::shared_ptr<Effect>>& getOnUseSlotObjectEffects() const { return onUseSlotObjectEffects; }
		void setOnUseSlotObjectEffects(std::vector<std::shared_ptr<Effect>> newEffects) { onUseSlotObjectEffects = newEffects; }

		const std::vector<std::shared_ptr<Condition>>& getOnEquipObjectConditions() const { return onEquipObjectConditions; }
		void setOnEquipObjectConditions(std::vector<std::shared_ptr<Condition>> newConditions) { onEquipObjectConditions = newConditions; }

		const std::vector<std::shared_ptr<Condition>>& getOnAddedInventoryObjectConditions() const { return onAddedInventoryObjectConditions; }
		void setOnAddedInventoryObjectConditions(std::vector<std::shared_ptr<Condition>> newConditions) { onAddedInventoryObjectConditions = newConditions; }

		const std::vector<std::shared_ptr<Condition>>& getOnUseInventoryObjectConditions() const { return onUseInventoryObjectConditions; }
		void setOnUseInventoryObjectConditions(std::vector<std::shared_ptr<Condition>> newConditions) { onUseInventoryObjectConditions = newConditions; }

		const std::vector<std::shared_ptr<Condition>>& getOnUseSlotObjectConditions() const { return onUseSlotObjectConditions; }
		void setOnUseSlotObjectConditions(std::vector<std::shared_ptr<Condition>> newConditions) { onUseSlotObjectConditions = newConditions; }

		const std::vector<std::string>& getSlotsUsed() const { return slotsUsed; }
		void setSlotsUsed(std::vector<std::string> newSlotsUsed) { slotsUsed = newSlotsUsed; }

		int getRequiredTechID() const { return requiredTechnologyID; }
		void setRequiredTechID(int newID) { requiredTechnologyID = newID; }

		double getContinuousActionTime() const { return continuousActionTime; }
		void setContinuousActionTime(double d) { continuousActionTime = d; }

		double getLoSRange() const { return lineOfSightRange; }
		void setLoSRange(double d) { lineOfSightRange = d; }

		int getInventorySize() const { return inventorySize; }
		void setInventorySize(int d) { inventorySize = d; }


		const std::unordered_map<ParameterID, Parameter>& getParameters() const { return parameters; }
		std::unordered_map<ParameterID, Parameter>& getParameters() { return parameters; }

		const std::vector<int>& getActionIDs() const { return actionIds; }
		std::vector<int>& getActionIDs() { return actionIds; }
		
		const std::vector<int>& getInitialObjectIDs() const { return initialObjectsIds; }
		std::vector<int>& getInitialObjectIDs() { return initialObjectsIds; }

		const std::vector<int>& getOnTickActionIDs() const { return onTickActionIds; }
		std::vector<int>& getOnTickActionIDs() { return onTickActionIds; }

		const std::vector<int>& getOnTickObjectActionIDs() const { return onTickObjectActionsIds; }
		std::vector<int>& getOnTickObjectActionIDs() { return onTickObjectActionsIds; }

		const std::unordered_set<EntityTypeID>& getSpawnableEntityTypes() const { return spawnableEntityTypes; }
		void setSpawnableEntityTypes(const std::unordered_set<EntityTypeID>& set) { spawnableEntityTypes = set; }

		const std::unordered_map<ParameterID, double>& getCosts() const { return cost; }
		void setCosts(const std::unordered_map<ParameterID, double>& c) { cost = c; }

		const std::unordered_set<int>& getCanEquipGroupEntityTypes() const { return canEquipGroupEntityTypes; }
		void setCanEquipGroupEntityTypes(const std::unordered_set<int>& groupTypes) { canEquipGroupEntityTypes = groupTypes; }
	};
}
