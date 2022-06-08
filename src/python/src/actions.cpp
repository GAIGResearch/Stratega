#include "headers.h"

namespace stratega
{
	void actions(py::module_& m)
	{
		py::class_<std::vector<SGA::Action>>(m, "ActionList","Lists of actions,  Contains the main information of an action as the action type id that is linked to an ActionType")
			.def(py::init<>())
			.def("__copy__", [](const std::vector<SGA::Action>& self) {
			return std::vector<SGA::Action>(self);
		})
			.def("__deepcopy__", [](const std::vector<SGA::Action>& self, py::dict) {
			return std::vector<SGA::Action>(self);
		})
			.def("clear", &std::vector<SGA::Action>::clear)
			.def("__getitem__", [](const std::vector<SGA::Action>& v, int index)
		{
			return v[index];
		})
			.def("pop_back", &std::vector<SGA::Action>::pop_back)
			.def("count", [](const std::vector<SGA::Action>& v) { return v.size(); })
			.def("__len__", [](const std::vector<SGA::Action>& v) { return v.size(); })
			.def("__iter__", [](std::vector<SGA::Action>& v) {
			return py::make_iterator(v.begin(), v.end());
		}, py::keep_alive<0, 1>());


		// ---- ActionFlag ----
		py::enum_<SGA::ActionFlag>(m, "ActionFlag", " Used to define how the SGA::ForwardModel::executeAction() will execute this action.")
			.value("None", SGA::ActionFlag::None)
			.value("EndTickAction", SGA::ActionFlag::EndTickAction)
			.value("ContinuousAction", SGA::ActionFlag::ContinuousAction)
			.value("AbortContinuousAction", SGA::ActionFlag::AbortContinuousAction)
			.export_values();

		// ---- ActionInfo ----
		py::class_<SGA::ActionInfo>(m, "ActionInfo", "Contains the last tick this action was executed and the action type ID. Used by forward models to check if the SGA::Entity or player can execute the action again after the cooldown.")
			.def(py::init<>())
			.def_readwrite("action_type_id", &SGA::ActionInfo::actionTypeID)
			.def_readwrite("last_executed_tick", &SGA::ActionInfo::lastExecutedTick);

		// ---- Action ----	
		py::class_<SGA::Action>(m, "Action", "Contains the main information of an action as the action type id that is linked to an ActionType")
			.def("__copy__", [](const SGA::Action& self) {
			return SGA::Action(self);
		})
			.def("__deepcopy__", [](const SGA::Action& self, py::dict) {
			return SGA::Action(self);
		})
			.def(py::init<const SGA::ActionType*>(), py::arg("actionType"))

			.def("execute", &SGA::Action::execute, py::arg("state"), py::arg("fm"), "Execute the effects defined in the SGA::ActionType linked to this action.")
			.def("validate", &SGA::Action::validate, py::arg("state"), "Checks if this action can be executed. It verifies if the conditions defined in the SGA::ActionType linked to this action are passed. This method also checks if the last time of the action execution is higher than the cooldown, and if all the preconditions and target conditions are fullfilled. It also checks that all the actions targets are valid.")
			.def("is_entity_action", &SGA::Action::isEntityAction, "Checks if this action is to be executed over an entity.")
			.def("is_player_action", &SGA::Action::isPlayerAction, "Checks if this action is to be executed over a player.")
			.def("get_source_id", &SGA::Action::getSourceID, "Gets the player or entity ID of the target of this action")
			.def("get_action_type_id", &SGA::Action::getActionTypeID, "Returns the action type ID of this action")
			.def("get_action_name", &SGA::Action::getActionName, "Returns the name of this action")
			.def("get_action_type", &SGA::Action::getActionType, "Checks if this action is to be executed over an entity.")
			.def("get_action_source_type", &SGA::Action::getActionSourceType, "Checks if this action is to be executed over an entity.")

			.def_static("create_end_action", &SGA::Action::createEndAction, py::arg("playerID"), py::arg("newActionType"), "Generates an Action used by the game to end the tick/turn.")
			.def_static("create_abort_entity_action", &SGA::Action::createAbortEntityAction, "Generates an Action which the owner is a entity, used by the game to abort a continuous action.")
			.def_static("create_abort_player_action", &SGA::Action::createAbortPlayerAction, "Generates an Action which the owner is a player, used by the game to abort a continuous action.")

			.def_property("max_value", &SGA::Action::getOwnerID, &SGA::Action::setOwnerID)
			.def("get_owner_id", &SGA::Action::getOwnerID)
			.def("set_owner_id", &SGA::Action::setOwnerID, py::arg("id"))

			.def_property("continuous_action_id", &SGA::Action::getContinuousActionID, &SGA::Action::setContinuousActionID)
			.def("get_continuous_action_id", &SGA::Action::getContinuousActionID)
			.def("set_continuous_action_id", &SGA::Action::setContinuousActionID, py::arg("id"))

			.def_property("elapsed_ticks", &SGA::Action::getElapsedTicks, &SGA::Action::setElapsedTicks)
			.def("get_elapsed_ticks", &SGA::Action::getElapsedTicks)
			.def("incTicks", &SGA::Action::incTicks)
			.def("set_elapsed_ticks", &SGA::Action::setElapsedTicks, py::arg("elapsed"))

			.def_property("action_flag", &SGA::Action::getActionFlag, &SGA::Action::setActionFlag)
			.def("get_action_flag", &SGA::Action::getActionFlag)
			.def("set_action_flag", &SGA::Action::setActionFlag, py::arg("flag"))

			.def("get_targets",
				[](SGA::Action& action)
		{
			return action.getTargets();
		}
			)
			.def("set_action_targets", &SGA::Action::setActionTargets, py::arg("actionTargets"))

			;

		// --- ActionSourceType ---
		py::enum_<SGA::ActionSourceType>(m, "ActionSourceType", "Used to know who is the source of an action, it can be a entity or a player.")
			.value("Entity", SGA::ActionSourceType::Entity)
			.value("Building", SGA::ActionSourceType::Player)
			.export_values();

		// --- ActionSourceTypeEnum ---
		py::enum_<SGA::TargetType::Type>(m, "TargetTypeEnum", "Target type: None, Position, Tile, EntityType, Entity, Technology, ContinuousAction, Object, SlotObject")
			.value("None", SGA::TargetType::Type::None)
			.value("Position", SGA::TargetType::Type::Position)
			.value("EntityType", SGA::TargetType::Type::EntityType)
			.value("Entity", SGA::TargetType::Type::Entity)
			.value("Technology", SGA::TargetType::Type::Technology)
			.value("ContinuousAction", SGA::TargetType::Type::ContinuousAction)
			.value("SlotObject", SGA::TargetType::Type::SlotObject)
			.value("Object", SGA::TargetType::Type::Object)
			.export_values();

		// ---- TargetType ----	
		py::class_<SGA::TargetType>(m, "TargetType", "Target type: None, Position, Tile, EntityType, Entity, Technology, ContinuousAction, Object, SlotObject")
			.def(py::init<>())

			.def("get_type_string", &SGA::TargetType::getTypeString)
			.def("is_valid", &SGA::TargetType::isValid, py::arg("state"), py::arg("actionTarget"), py::arg("sourceActionTarget"))

			.def_property("type", &SGA::TargetType::getType, &SGA::TargetType::setType)
			.def("get_type", &SGA::TargetType::getType)
			.def("set_type", &SGA::TargetType::setType, py::arg("type"))

			.def_property("sampling_method", &SGA::TargetType::getSamplingMethod, &SGA::TargetType::setSamplingMethod)
			.def("get_sampling_method", &SGA::TargetType::getSamplingMethod)
			.def("set_sampling_method", &SGA::TargetType::setSamplingMethod, py::arg("samplingMethod"))

			.def_property("group_entity_types", &SGA::TargetType::getGroupEntityTypes, &SGA::TargetType::setGroupEntityTypes)
			.def("get_group_entity_types", &SGA::TargetType::getGroupEntityTypes)
			.def("set_group_entity_types", &SGA::TargetType::setGroupEntityTypes, py::arg("groupTypes"))

			.def("get_technology_types",
				[](SGA::TargetType& targetType)
		{
			return targetType.getTechnologyTypes();
		}
			)
			;


		// --- ActionTargetEnum ---
		py::enum_<SGA::ActionTarget::Type>(m, "ActionTargetEnum", "Used by Action to reference any possible type of target available in Stratega.")
			.value("Position", SGA::ActionTarget::Type::Position)
			.value("EntityReference", SGA::ActionTarget::Type::EntityReference)
			.value("PlayerReference", SGA::ActionTarget::Type::PlayerReference)
			.value("EntityTypeReference", SGA::ActionTarget::Type::EntityTypeReference)
			.value("TechnologyReference", SGA::ActionTarget::Type::TechnologyReference)
			.value("ContinuousActionReference", SGA::ActionTarget::Type::ContinuousActionReference)
			.value("TileTypeReference", SGA::ActionTarget::Type::TileTypeReference)
			.value("Gamestate", SGA::ActionTarget::Type::Gamestate)
			.value("SlotObject", SGA::ActionTarget::Type::SlotObject)
			.value("Object", SGA::ActionTarget::Type::Object)
			.export_values();

		// ---- ActionTarget ----	
		py::class_<SGA::ActionTarget>(m, "ActionTarget", "Used by Action to reference any possible type of target available in Stratega.")
			.def_static("create_position_action_target", &SGA::ActionTarget::createPositionActionTarget, py::arg("position"))
			.def_static("create_entity_action_target", &SGA::ActionTarget::createEntityActionTarget, py::arg("entityID"))
			.def_static("create_object_target", &SGA::ActionTarget::createObjectActionTarget, py::arg("entityID"))
			.def_static("create_slot_object_action_target", &SGA::ActionTarget::createSlotObjectActionTarget, py::arg("entityID"))
			.def_static("create_player_action_target", &SGA::ActionTarget::createPlayerActionTarget, py::arg("playerID"))
			.def_static("create_technology_entity_action_target", &SGA::ActionTarget::createTechnologyEntityActionTarget, py::arg("technologyID"))
			.def_static("create_continuous_action_target", &SGA::ActionTarget::createContinuousActionActionTarget, py::arg("continuousActionID"))
			.def_static("create_entity_type_action_target", &SGA::ActionTarget::createEntityTypeActionTarget, py::arg("entityTypeID"))
			.def_static("create_tile_type_action_target", &SGA::ActionTarget::createTileTypeActionTarget, py::arg("entityTypeID"))

			.def("get_entity_const", &SGA::ActionTarget::getEntityConst, py::arg("state"))
			.def("get_entity", &SGA::ActionTarget::getEntity, py::arg("state"))
			.def("get_player", &SGA::ActionTarget::getPlayer, py::arg("state"))
			.def("get_player_const", &SGA::ActionTarget::getPlayerConst, py::arg("state"))
			.def("get_entity_type", &SGA::ActionTarget::getEntityType, py::arg("state"))
			.def("get_tile_type", &SGA::ActionTarget::getTileType, py::arg("state"))
			.def("get_spawneable_entities", &SGA::ActionTarget::getSpawnableEntities, py::arg("state"))

			//.def("get_position", &SGA::ActionTarget::getPosition, py::arg("state"))
			.def("get_technology_id", &SGA::ActionTarget::getTechnologyID)
			.def("get_player_id", py::overload_cast<const SGA::GameState&>(&SGA::ActionTarget::getPlayerID, py::const_))
			.def("get_player_id", py::overload_cast<>(&SGA::ActionTarget::getPlayerID, py::const_))
			.def("get_entity_id", &SGA::ActionTarget::getEntityID)
			.def("get_object_id", &SGA::ActionTarget::getObjectID)
			.def("get_continuous_action_id", &SGA::ActionTarget::getContinuousActionID)
			.def("get_type", &SGA::ActionTarget::getType)
			.def("get_type_string", &SGA::ActionTarget::getTypeString)
			.def("get_value_string", &SGA::ActionTarget::getValueString, py::arg("state"))

			.def_static("is_valid_with_targets", &SGA::ActionTarget::isValidWithTargets, py::arg("state"), py::arg("actionType"), py::arg("actionTargets"))
			.def("is_valid", &SGA::ActionTarget::isValid, py::arg("state"))
			;

		// ---- Action Assignment ----
		py::class_<SGA::ActionAssignment>(m, "ActionAssignment", "A collection of action-assignments to entities and players in a game.")
			.def("__copy__", [](const SGA::ActionAssignment& self) {
			return SGA::ActionAssignment(self);
		})
			.def("__deepcopy__", [](const SGA::ActionAssignment& self, py::dict) {
			return SGA::ActionAssignment(self);
		})
			.def(py::init<>())
			.def("assign_action_or_replace", &SGA::ActionAssignment::assignActionOrReplace, py::arg("newAction"), "Assigns the action to the corresponding entity or player. Any existing assignment will be overwritten.")
			.def("merge", &SGA::ActionAssignment::merge, py::arg("merge"), "Copies the given action-assignment into this object. Any existing assignment will be overwritten in case of a conflict.")
			.def("clear", &SGA::ActionAssignment::clear, "Deletes all assignments.")
			.def("get_entity_action", &SGA::ActionAssignment::getEntityAction, py::arg("entityID"), " Returns the assignment for an entity.")
			.def("get_player_action", &SGA::ActionAssignment::getPlayerAction, py::arg("playerID"), "Returns the assignment for an player.")
			.def("get_assignment_count", &SGA::ActionAssignment::getAssignmentCount, "Returns the amount of actions that are assigned to entities and players.")
			.def("get_entity_actions", &SGA::ActionAssignment::getEntityActions)
			.def("get_player_actions", &SGA::ActionAssignment::getPlayerActions)
			.def_static("from_single_action", &SGA::ActionAssignment::fromSingleAction, py::arg("a"), "Constructs a ActionAssignment from a single action, useful for TBS-Games.")
			.def_static("create_end_action_assignment", &SGA::ActionAssignment::createEndActionAssignment, py::arg("playerID"), "Generates an ActionAssignment used by the game to end the tick/turn.");


		// ---- Action type ----
		py::class_<SGA::ActionType, std::shared_ptr<SGA::ActionType>>(m, "ActionType", "Contains the definition of an action. Which type is the source SGA::ActionSourceType , a set effects,preconditions and a list of action targets linked to their target conditions.If the action is continuous it will have a set of effects as events:OnStart, OnTick, OnComplete, OnAbort and the list of conditions that triggers the completed action.")
			.def(py::init<>())
			.def("get_target_conditions", &SGA::ActionType::getTargetConditions, py::arg("searchingTarget"), "Returns a list of conditions linked to the searched target.")

			.def_property("name", &SGA::ActionType::getName, &SGA::ActionType::setName)
			.def("get_name", &SGA::ActionType::getName, "Name of this action type.")
			.def("set_name", &SGA::ActionType::setName, py::arg("name"), "Name of this action type.")

			.def_property("id", &SGA::ActionType::getID, &SGA::ActionType::setID)
			.def("get_id", &SGA::ActionType::getID, "Unique ID of this action type")
			.def("set_id", &SGA::ActionType::setID, py::arg("id"), "Unique ID of this action type")

			.def_property("source_type", &SGA::ActionType::getSourceType, &SGA::ActionType::setSourceType)
			.def("get_source_type", &SGA::ActionType::getSourceType, "Source of this action (entity, player...)")
			.def("set_source_type", &SGA::ActionType::setSourceType, py::arg("ast"), "Source of this action (entity, player...)")

			.def_property("cooldown", &SGA::ActionType::getCooldown, &SGA::ActionType::setCooldown)
			.def("get_cooldown", &SGA::ActionType::getCooldown, "Number of ticks/turn that must happen between two uses of this action.")
			.def("set_Cooldown", &SGA::ActionType::setCooldown, py::arg("cd"), "Number of ticks/turn that must happen between two uses of this action.")

			.def_property("continuous", &SGA::ActionType::isContinuous, &SGA::ActionType::setContinuous)
			.def("is_continuous", &SGA::ActionType::isContinuous)
			.def("set_continuous", &SGA::ActionType::setContinuous, py::arg("c"))

			.def("get_preconditions",
				[](SGA::ActionType& actionType)
		{
			return actionType.getPreconditions();
		}
				, "List of preconditions for this action to be run. These are tier 1 conditions: if they can't be fulfilled the action is not possible. If the conditions are true, the action may be possible based on situational circumstances.  (Example: having a resource to create an entity but no space in the board to spawn it)."
			)
			.def("get_targets",
				[](SGA::ActionType& actionType)
		{
			return actionType.getTargets();
		}
				, "List of target types of this action with their conditions. Each target is a pair <target, list<conditions>>. These are tier 2 conditions (i.e. situational), to be checked during action generation."
			)
			.def("get_trigger_complete",
				[](SGA::ActionType& actionType)
		{
			return actionType.getTriggerComplete();
		}
				, "Continuous actions finish when certain conditions are met. This vector indicates those conditions."
			)
			.def("get_effects",
				[](SGA::ActionType& actionType)
		{
			return actionType.getEffects();
		}
				, " List of effects this action causes when executed. This list is populated only for non continuous actions."
			)
			.def("get_on_start",
				[](SGA::ActionType& actionType)
		{
			return actionType.getOnStart();
		}
				, "For continuous actions, list of effects that are applied when this action starts."
			)
			.def("get_on_tick",
				[](SGA::ActionType& actionType)
		{
			return actionType.getOnTick();
		}
				, "For continuous actions, list of effects that are applied on every tick/turn. This effect is *not* applied on the tick this action starts, but it *will* also be applied in the tick it ends."
			)
			.def("get_on_complete",
				[](SGA::ActionType& actionType)
		{
			return actionType.getOnComplete();
		}
				, "For continuous actions, list of effects that are applied when this action ends."
			)
			.def("get_on_abort",
				[](SGA::ActionType& actionType)
		{
			return actionType.getOnAbort();
		}
				, "For continuous actions, list of effects that are applied when this action is aborted. Aborted actions do not execute the OnTick effect in the game tick they have been aborted."
			)
			;

	}	
}