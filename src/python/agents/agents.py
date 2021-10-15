import collections.abc

import stratega
import os
import sys
import random
import copy
import time
from contextlib import contextmanager
import fnmatch
#------------------------------ Agents ----------------------------------------
class DoNothingPythonAgent(stratega.Agent):
    def init(self, state, forward_model, timer):
        print("init")

    def compute_action(self, state, forward_model, timer):

        print("run DoNothingAgent")
        return stratega.ActionAssignment.create_end_action_assignment(self.get_player_id())


class RandomPythonAgent(stratega.Agent):
    def init(self, state, forward_model, timer):
        print("init")

    def compute_action(self, state, forward_model, timer):

        print("run RandomAgent")
        actions=forward_model.generate_actions(state, self.get_player_id())
        action=actions.__getitem__(random.randint(0, actions.count()-1))

        action_assignment=stratega.ActionAssignment.from_single_action(action)

        return action_assignment

def evaluate_state(state, player_id):
    score=0.0
    opponent_entites=state.get_non_player_entities(player_id, stratega.EntityCategory.Null)
    player_entites=state.get_player_entities(player_id, stratega.EntityCategory.Null)

    if state.is_game_over() and state.get_winner_id() == player_id:
       score=1000
    elif state.is_game_over() and state.get_winner_id() != player_id:
        score=-1000

    sum_of_average_distances = 0
    if not player_entites:
        for p in player_entites:
            sum_of_distances=0
            for o in opponent_entites:
                sum_of_distances+= abs(p.position.x-o.position.x)+ abs(p.position.y-o.position.y)

            sum_of_average_distances=sum_of_distances/len(opponent_entites)

    if not player_entites:
        score += sum_of_average_distances/len(player_entites)

    return -score


class OSLAPythonAgent(stratega.Agent):
    def __init__(self):
        stratega.Agent.__init__(self, "OSLAPythonAgent")

    def init(self, state, forward_model, timer):
        print("init")

    def compute_action(self, state, forward_model, timer):

        print("run OSLAAgent")
        actions = forward_model.generate_actions(state, self.get_player_id())
        best_heuristic_value=-float("inf")
        best_action_index=0

        for index, action in enumerate(actions):
            gs_copy = copy.deepcopy(state)
            forward_model.advance_gamestate(gs_copy, action)

            value=evaluate_state(gs_copy, self.get_player_id())

            if value > best_heuristic_value:
                best_heuristic_value = value
                best_action_index = index
        action=actions.__getitem__(best_action_index)
        action_assignment=stratega.ActionAssignment.from_single_action(action)
        return action_assignment


def run_arena():
    config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")

    number_of_games=1
    player_count=2
    log_path="./sgaLog3.yaml"
    maps_path=""
    seed=0

    stratega.set_default_logger(log_path)
    arena=stratega.create_arena(config)

    if not maps_path:
        #arena.run_games(player_count,seed, number_of_games,1)
        #run with python agents
        arena.run_games(player_count, seed, number_of_games,1,[RandomPythonAgent(), OSLAPythonAgent()])
    else:
        config.level_definitions= stratega.load_levels_from_yaml(maps_path, config)
        map_number=len(config.level_definitions)
        #arena.run_games(player_count, seed, number_of_games,map_number)
        #run with python agents
        arena.run_games(player_count, seed, number_of_games,map_number, ["CombatAgent", OSLAPythonAgent()])

def play_gui():
    # local python module resources
    config = stratega.load_config("resources/gameConfigurations/TBS/KillTheKing.yaml")

    runner = stratega.create_runner(config)

    # Config agents
    config_agents = stratega.generate_agents(config)

    # --- Play/Run -----
    runner.play(config_agents, stratega.Vector2i(1200,800), 0)
    #runner.play(["HumanPlayer", OSLAPythonAgent()], stratega.Vector2i(1920,1080), 0)


def run_clean():
    #run_arena()
    play_gui()

#------------------------------- Main -----------------------------------------
if __name__ == "__main__":
    run_clean()