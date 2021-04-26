#############
Forward Models and Actions
#############

The forward model is the driving component of this framework.
Being provided with a game-state it can generate a new set of available actions thanks to the ActionSpace object.
It handles the efficient creation of available actions and updates the set of actions after the game-state has been updated.
Returned action objects store the type of action, the executing unit and the target tile.
Nevertheless, they do not include the action's logic which remains part of the forward model.

When a player provides the forward model with an action and a game-state, it first checks if the action is applicable.
This needs to be done, since the player may have modified either the action or the game-state in between generating the set of actions and simulating its outcome.
These security checks can be deactivated to increase the framework's speed. 
In case the action can be executed, the game-state object will be advanced accordingly.

Implemented actions include a movement and an attack action as well as special abilities such as healing units among others.
The way in which actions are to be executed is defined in the forward model, which is set up using the configuration files.
This allows to quickly change the action's parameters, and therefore, create variants of the same game.
This approach has been chosen to easily modify the game's balance without updating the code.

Additional flexibility is achieved by the use of triggers and effects.
Given an event, e.g. a unit entering a cell or a player ending a turn, the forward model can execute subscribed methods.
This can be used to implement game-mechanics such as a unit losing health at the end of each turn.
Alternatively, it allows to log information every time an event occurs.

Both, the forward model and the game state, can be extended by the user to create game-state abstractions.
This can be useful for reducing the complexity of the game-states encoding and ease the training of AI agents.
In the future, we will implement a vector-based state observation to better support the development of reinforcement learning agents.
Similarly, search-based agents may achieve higher performance due to the reduced search space of such an abstraction.