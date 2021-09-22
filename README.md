# Stratega
[![Documentation Status](https://readthedocs.org/projects/stratega/badge/?version=latest)](https://stratega.readthedocs.io/en/latest/?badge=latest)
[![pypi releases](https://img.shields.io/pypi/v/stratega.svg)](https://pypi.org/project/stratega)
[![Builds](https://github.com/GAIGResearch/Stratega/actions/workflows/ci.yml/badge.svg?branch=dev)](https://github.com/GAIGResearch/Stratega/actions/workflows/ci.yml)
[![Python Wheel Builds](https://github.com/GAIGResearch/Stratega/actions/workflows/wheels.yml/badge.svg?branch=dev)](https://github.com/GAIGResearch/Stratega/actions/workflows/wheels.yml)

The Stratega framework mainly consists of 4 parts, i.e. the game-runner, rendering, agents, and configuration. The core of the framework is the game-runner, whose sole purpose is to run games and provide informationabout the game. The framework provides two game-runners, one for turn-based and one for RTS-games. The agents are responsible for controlling the actions of a player in the game state. To do this, each agent receives a game communicator to send actions or poll information about the current game state. Additionally, each agent has access to a copy of the forward model of the game-runner. Regardless of the type of game,every agent-thread runs until the game is over. Agents can query information or send actions at any point intime, even during an opponent’s turn. This enables agents to think outside their own turns. Additionally,the agent can observe how the game evolves to think ahead. Note that the turn-based game-runner ignores actions from players when it’s not their turn. Meanwhile, the RTS-runner will collect the actions from allplayers and execute them in order of receipt after a set amount of time. The runner keeps track of which agent controls which player in the game state. If no agent is available fora player, the runner will send a signal and wait until an external source provides the missing actions. The render-thread subscribes to events from the game-runner to update the window. If the runner signals missing agents, the GUI will switch to interactive mode to let human-players play the game. That also has the added benefit that human-players have no time limit. This approach has similarities to MVVM, in the sense that the game-runner represents the view-model. The renderer (view) is decoupled from the logic and can be easilydisabled to switch between headless-execution and GUI-execution. Stratega aims to provide a fast and flexible framework for researching AI in complex strategy games. Games are configured using YAML-files and can be played through a GUI or by agents using an API. Stratega allows creating a wide variety of turn-based and real-time strategy games. Due to the flexibility of using YAML-files, the user can design and run various games for testing agents without adjusting it to the game. The framework has been built with a focus of statistical forward planning (SFP) agents. For this purpose, agents can access and modify game states and use the forward model to simulate the game. Thanks to the ability to configure a wide range of games and access to the forward model, Stratega is perfectly suited for researching general game playing in complex games.

# Community

Join the Discord community for help and to talk about what you are doing with Stratega!
[![Discord Chat](https://img.shields.io/discord/783231009738719233.svg)](https://discord.gg/Y2uZZ3TSuT)

## Documentation

Full documentation can be found here:
[stratega.readthedocs.io](https://stratega.readthedocs.io/)

# Gallery
TBS
![TBS](/images/tbsScreenshot.png)
RTS
![RTS](/images/rtsScreenshot.png)
