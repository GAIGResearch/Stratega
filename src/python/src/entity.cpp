#include "headers.h"

namespace stratega
{
	void entity(py::module_& m)
	{
		// ---- Entity type ----
		py::class_<SGA::EntityType, std::shared_ptr<SGA::EntityType>>(m, "EntityType", "Contains the definition of the entity type. This information includes a list of parameters definitions, the actions that can execute, required technologies to spawn a entity, a list of spawnable entity types and a list of cost assigned to this entity type.")
			.def(py::init<>())

			.def("get_parameter", &SGA::EntityType::getParameter, py::arg("id"), "Returns a SGA::Parameter reference that contains the parameter defintion")
			.def("can_execute_action", &SGA::EntityType::canExecuteAction, py::arg("actionTypeID"), "Checks if this entity type is allowed to execute an action type")
			.def("instantiate_entity", &SGA::EntityType::instantiateEntity, py::arg("entityID"), "Generate a new empty instance of this entity type ")
			.def("get_param_max", &SGA::EntityType::getParamMax, py::arg("paramName"), "Returns the maximum value of a given parameter")
			.def("get_param_min", &SGA::EntityType::getParamMin, py::arg("paramName"), " Returns the minimum value of a given parameter")

			.def_property("id", &SGA::EntityType::getID, &SGA::EntityType::setID)
			.def("get_id", &SGA::EntityType::getID, "Unique ID of this entity type")
			.def("set_id", &SGA::EntityType::setID, py::arg("id"), "Unique ID of this entity type")

			.def_property("name", &SGA::EntityType::getName, &SGA::EntityType::setName)
			.def("get_name", &SGA::EntityType::getName, "Name of this type, as defined in the YAML file.")
			.def("set_name", &SGA::EntityType::setName, py::arg("name"), "Name of this type, as defined in the YAML file.")

			.def_property("symbol", &SGA::EntityType::getSymbol, &SGA::EntityType::setSymbol)
			.def("get_symbol", &SGA::EntityType::getSymbol, "Symbol for this entity used in YAML for level placement.")
			.def("set_symbol", &SGA::EntityType::setSymbol, py::arg("s"), "Symbol for this entity used in YAML for level placement.")

			.def_property("required_tech_id", &SGA::EntityType::getRequiredTechID, &SGA::EntityType::setRequiredTechID)
			.def("get_required_tech_id", &SGA::EntityType::getRequiredTechID, "ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.")
			.def("set_required_tech_id", &SGA::EntityType::setRequiredTechID, py::arg("id"), "ID of the technology that needs to be researched so this entity can be spawned/built/created in the game.")

			.def_property("continuous_action_time", &SGA::EntityType::getContinuousActionTime, &SGA::EntityType::setContinuousActionTime)
			.def("get_continuous_action_time", &SGA::EntityType::getContinuousActionTime, "Continuous Action Time")
			.def("set_continuous_action_time", &SGA::EntityType::setContinuousActionTime, py::arg("d"), "Continuous Action Time")

			.def_property("line_of_sight_range", &SGA::EntityType::getLoSRange, &SGA::EntityType::setLoSRange)
			.def("get_line_of_sight_range", &SGA::EntityType::getLoSRange, "Range of the line of sight of this unit.")
			.def("set_line_of_sight_range", &SGA::EntityType::setLoSRange, py::arg("d"), "Range of the line of sight of this unit.")

			.def("get_parameters",
				[](SGA::EntityType& entityType)
		{
			return entityType.getParameters();
		}
				, "Map of parameters of this entity. Key is an integer (ID of the parameter) and value is the paramter object. The parameter defines the min, max and default value this parameter can take."
			)

			.def("get_action_ids",
				[](SGA::EntityType& entityType)
		{
			return entityType.getActionIDs();
		}
				, "List of action IDs this entity can execute."
			)
			.def("get_slots",
				[](SGA::EntityType& entityType)
		{
			return entityType.getSlots();
		}
				, "List of slots this entity has."
			)
			.def("get_spawnable_entity_types",
				[](SGA::EntityType& entityType)
		{
			return entityType.getSpawnableEntityTypes();
		}
				, "For continuous actions, list of effects that are applied when this action is aborted. Aborted actions do not execute the OnTick effect in the game tick they have been aborted."
			)
			.def("get_costs",
				[](SGA::EntityType& entityType)
		{
			return entityType.getCosts();
		}
				, "List of parameters and values that must be spent to spawn/build/create this unit."
			)

			;

		// ---- Entity ----
		py::class_<SGA::Entity>(m, "Entity", "Is an object that interacts with the game and responds to player input or other entities. This entity can be assigned to a player performing a set of actions per tick/turn. It has a list of parameters that store real values. If the player wants to check the SGA::EntityType it can search it using the assigned typeID.")
			.def("__copy__", [](const SGA::Entity& self) {
			return SGA::Entity(self);
		})
			.def("__deepcopy__", [](const SGA::Entity& self, py::dict) {
			return SGA::Entity(self);
		})
			.def(py::init<>())
			.def("init", &SGA::Entity::init, py::arg("type"), py::arg("entityID"), "Initializes the entity with a given ID. Creates default attached actions and parameters list from the entity type.")

			.def("get_entity_type", &SGA::Entity::getEntityType, "Return entity type")
			.def("get_entity_type_id", &SGA::Entity::getEntityTypeID, "Return entity type id")
			.def("is_neutral", &SGA::Entity::isNeutral, "Indicates if this unit is neutral (i.e. does not belong to any playing faction).")
			.def("get_action_info", &SGA::Entity::getActionInfo, py::arg("actionTypeID"), "Returns a SGA::ActionInfo object linked to action type,")
			.def("get_attached_actions", &SGA::Entity::getAttachedActions, "Returns the actions attached to this entity.")
			.def("get_action_types", &SGA::Entity::getActionTypes, py::arg("gameInfo"), "Retrieves the list of action types this entity can execute.")
			.def("set_action_ticks", &SGA::Entity::setActionTicks, py::arg("actionTypeID"), py::arg("tick"), "Sets the last executed game ticks to the tick passed by parameter for the action corresponding to the type received.")
			.def("flag_remove", &SGA::Entity::flagRemove, "Flags this entity to be removed by the game engine.")
			.def("flagged", &SGA::Entity::flagged, "Checks if this entity should be removed by the game engine.")

			.def("print_info", &SGA::Entity::printInfo, "Prints information about the entity, parameters and actions")
			.def("get_param_values", py::overload_cast<>(&SGA::Entity::getParamValues, py::const_), "Gets the list of parameters values. Modifiable.")
			.def("get_param_values", py::overload_cast<>(&SGA::Entity::getParamValues), "Gets the list of parameters values.")
			.def("get_buffs", py::overload_cast<>(&SGA::Entity::getBuffs, py::const_), "Gets the list of buffs. Modifiable.")
			.def("get_buffs", py::overload_cast<>(&SGA::Entity::getBuffs), "Gets the list of buffs.")
			.def("get_entity_parameter_names", &SGA::Entity::getEntityParameterNames, "Gets the list of parameters names.")
			.def("get_entity_parameters", &SGA::Entity::getEntityParameters, " Gets a map with all pairs <parameter,value>")
			.def("get_parameter", &SGA::Entity::getParameter, py::arg("paramName"), "Gets the value of a specific parameter, by name ")
			.def("get_parameter_at", &SGA::Entity::getParameterAt, py::arg("paramIdx"), "Gets a specific parameters, by index ")

			.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions, py::const_), "Gets the list of continuous actions attached to this entity. Modifiable.")
			.def("get_continuous_actions", py::overload_cast<>(&SGA::Entity::getContinuousActions), "Gets the list of continuous actions attached to this entity. ")

			.def("get_movement_speed", &SGA::Entity::getMovementSpeed, "Returns the movement speed of this entity.")
			.def("get_collision_radius", &SGA::Entity::getCollisionRadius, " Returns the collision radius of this entity.")
			.def("get_id", &SGA::Entity::getID, "Returns theID of this entity.")
			.def("get_owner_id", &SGA::Entity::getOwnerID, "Returns the owner ID (i.e. player ID) who controls this entity.")
			.def("set_owner_id", &SGA::Entity::setOwnerID, py::arg("oID"), "Returns the owner ID (i.e. player ID) who controls this entity.")
			.def("get_line_of_sight_range", &SGA::Entity::getLineOfSightRange, "Returns the line of sight of this entity.")
			.def("x", &SGA::Entity::x, "Returns y position of this entity.")
			.def("y", &SGA::Entity::y, "Returns y position of this entity.")
			.def("get_position", &SGA::Entity::getPosition, "Returns y position of this entity.")
			.def("set_position", &SGA::Entity::setPosition, py::arg("v"), "Sets the position of this entity in the board. Does not modify the board.")
			.def("get_path", &SGA::Entity::getPath, "Returns the path that this entity is following (RTS games only) ")
			.def("inc_path_index", &SGA::Entity::incPathIndex, "Increments the current index of the path that this entity is following (RTS games only) ")
			.def("set_path", &SGA::Entity::setPath, "Sets the path this entity is following (RTS games only) ")

			.def("get_inventory_size", &SGA::Entity::getInventorySize, "Gets the inventory size.")
			.def("get_inventory_use", &SGA::Entity::getInventoryUse, "Gets the inventory slots number used.")
			.def("is_inventory_use", &SGA::Entity::isInventoryFull, "Return true if the inventory is full.")
			.def("add_object", &SGA::Entity::addObject, py::arg("newObject"), "Adds object to the inventory.")
			.def("remove_object", &SGA::Entity::removeObject, py::arg("entityID"), "Removes object from the inventory.")
			.def("remove_slot_object", &SGA::Entity::removeSlotObject, py::arg("entityID"), "Removes object from slot.")
			.def("get_inventory", &SGA::Entity::getInventory, "Get inventory list of entities.")
			.def("get_slots", &SGA::Entity::getSlots, "Get slot list of entities.")
			.def("equip_object", &SGA::Entity::equipObject, py::arg("entityID"), "Equip object from the inventory.")
			.def("unequip_object", &SGA::Entity::unEquipObject, py::arg("entityID"), "Unequip object to the inventory.")
			.def("check_slots_are_in_use", &SGA::Entity::checkSlotsAreNotInUse, py::arg("object"), "Checks if slots that a object needs are not in use.")
			.def("has_object", &SGA::Entity::hasObject, py::arg("entityID"), "Return if the entity has a object with a specific Id.")
			.def("has_slot_object", &SGA::Entity::hasSlotObject, py::arg("entityID"), "Return if the entity has a slot object with a specific Id.")

			.def("get_object", &SGA::Entity::getObject, py::arg("entityID"), "Return the object of this entity with a given entityID.")
			.def("get_slot_object", &SGA::Entity::getSlotObject, py::arg("entityID"), "Return the slot object of this entity with a given entityID.")
			;

	}	
}