#include "headers.h"

namespace stratega
{
	void player(py::module_& m)
	{
		// ---- Player ----
		py::class_<SGA::Player>(m, "Player", "Contains all the implementation of a player. Usefull to check the attached actions and the parameters assigned to the player.")
			.def("__copy__", [](const SGA::Player& self) {
			return SGA::Player(self);
		})
			.def("__deepcopy__", [](const SGA::Player& self, py::dict) {
			return SGA::Player(self);
		})

			.def(py::init<int, bool>(), py::arg("id"), py::arg("canPlay"))

			.def("can_execute_action", &SGA::Player::canExecuteAction, py::arg("actionTypeID"), "Indicates if a given action type can be executed by this player.")
			.def("get_action_info", &SGA::Player::getActionInfo, py::arg("actionTypeID"), " Returns the ActionInfo of an action type this player can execute.")

			.def("get_neutral_player_id", &SGA::Player::getNeutralPlayerID, "Returns the player ID defined for a neutral player.")
			.def("get_id", &SGA::Player::getID, "Returns the ID of this player")
			.def("can_play", &SGA::Player::canPlay, "Indicates if this player can play at this time.")
			.def("set_can_play", &SGA::Player::setCanPlay, py::arg("can"), "Sets if the player can play now or not.")
			.def("get_parameters",
				[](SGA::Player& player)
		{
			return player.getParameters();
		}
				, "Returns the list of parameters, can't be modified."
			)
			.def("get_parameter",
				[](SGA::Player& player, int paramIdx)
		{
			return player.getRawParameterAt(paramIdx);
		}
				, "Returns a const value of a parameter of this player."
			)


			.def("set_parameter", &SGA::Player::setParameter, py::arg("paramIdx"), py::arg("val"), " Sets the parameter of this player to a certain value")
			//.def("resize_parameters", &SGA::Player::resizeParameters, py::arg("cap"), " Sets a size for the vector of parameters of this player.")
			.def("remove_continuous_action", &SGA::Player::removeContinuousAction, py::arg("idx"), " Sets a size for the vector of parameters of this player.")
			.def("add_continuous_action", &SGA::Player::addContinuousAction, py::arg("newAction"), " Adds a continuous action to the list of this player.")
			.def("advance_continuous_action", &SGA::Player::advanceContinuousAction, py::arg("idx"), "Advances the tick counter of the continuous action with index 'idx'")
			.def("get_continuous_actions", &SGA::Player::getContinuousActions, "Returns the list of continuous actions this player is executing.")

			.def("get_attached_actions", &SGA::Player::getAttachedActions, " Returns the list of attached actions to this player.")
			.def("get_attached_action", &SGA::Player::getAttachedAction, py::arg("idx"), " Returns the action info of the attached action with index 'idx'.")

			.def("add_attached_action", &SGA::Player::addAttachedAction, py::arg("actionTypeID"), py::arg("lastExecutedTick"), "Adds a new attached action to this player.")
			.def("set_action_last_tick", &SGA::Player::setActionLastTick, py::arg("idx"), py::arg("lastTick"), "Sets the last tick on an attached action, indexed by 'idx'")

			.def("res_attached_actions", &SGA::Player::resAttachedActions, py::arg("cap"), "Reserves space for attached actions.")
			;
	}	
}