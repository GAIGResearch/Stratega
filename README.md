﻿# Stratega
Stratega, a general strategy games framework, has been designed to foster research on computational intelligence for strategy games. In contrast to other strategy game frameworks, Stratega allows to create a wide variety of turn-based and real-time strategy games using acommon API for agent development. Flexibility is achieved by utilising YAML-files to configure tiles, units, actions, and levels. Therefore, the user can design and run a variety of games to test developed agents without specifically adjusting it to the game being generated. The framework has been built with a focus of statistical forward planning (SFP) agents. For this purpose, agents can access and modify game-states and use the forward model to simulate the outcome oftheir actions. While SFP agents have shown great flexibility in general game-playing, their performance is limited in case of complex state and action-spaces.

Stratega comes with a variety of pre-implemented game-modes:
  - No Name Game (most complex until now)
  - City Capturing
  - Kill The King
  - Protect The Base

## -Structure
The Stratega framework mainly consists of 4 parts, i.e. the game-runner, rendering, agents, and configuration. The core of the framework is the game-runner, whose sole purpose is to run games and provide informationabout the game. The framework provides two game-runners, one for turn-based and one for RTS-games. The agents are responsible for controlling the actions of a player in the game state. To do this, each agent receives a game communicator to send actions or poll information about the current game state. Additionally, each agent has access to a copy of the forward model of the game-runner. Regardless of the type of game,every agent-thread runs until the game is over. Agents can query information or send actions at any point intime, even during an opponent’s turn. This enables agents to think outside their own turns. Additionally,the agent can observe how the game evolves to think ahead. Note that the turn-based game-runner ignores actions from players when it’s not their turn. Meanwhile, the RTS-runner will collect the actions from allplayers and execute them in order of receipt after a set amount of time. The runner keeps track of which agent controls which player in the game state. If no agent is available fora player, the runner will send a signal and wait until an external source provides the missing actions. The render-thread subscribes to events from the game-runner to update the window. If the runner signals missing agents, the GUI will switch to interactive mode to let human-players play the game. That also has the added benefit that human-players have no time limit. This approach has similarities to MVVM, in the sense that the game-runner represents the view-model. The renderer (view) is decoupled from the logic and can be easilydisabled to switch between headless-execution and GUI-execution.


### -GUI (GameStateRender)
The GUI inherits from GameCommunicator and is in charge of drawing the current game state.
Run on its own thread and is completely detached from Stratega, if it has a played assigned the human player can play via GUI.
It provides utilities for easier control.


 
# Arena
 Easy way to test the performance between different Agents. It lets play a set of Agents versus each other.
Each battle use same seed (Gamestate generation)


### -Logging system
Stratega implements a simple mechanism for logging results into yaml files. Automatic logging is achieved by using Game Communicators.

Log Example:
```sh
Game:
  PlayerAssignment: [RandomAgent, RHEAAgent]
  ActivePlayer: [0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1]
  ActionCount: [31, 27, 16, 4, 3, 2, 55, 54, 42, 17, 5, 91, 31, 19, 7, 6, 72, 29, 17, 6, 2, 1, 88, 31, 27, 26, 13, 2, 1, 70, 30, 18, 91, 30, 26, 16, 4, 75, 31, 28, 17, 16, 71, 59, 19, 7, 4, 3, 2, 2, 2, 1, 28, 18, 7, 6, 3, 1, 74, 31, 30, 28, 16, 14, 14, 14, 13, 1, 16, 6, 85, 30, 29, 17, 6, 5, 5, 2]
  WinnerID: 1
  Turns: 7
```
 
### Performance
STRATEGA’s components have been optimized for speed, to allow as many simulations as possible per second. Using a Windows 10 x64, with CPU: I7-6700HQ 2.60GHz; RAM: 16Gb; GPU GTX960m the simulation allows for≈100 000 forward model calls per second in turn-based mode and ≈35 000 calls per second in real-time strategy mode.

# Gallery
TBS
![TBS](/images/tbsScreenshot.png)
RTS
![RTS](/images/rtsScreenshot.png)
