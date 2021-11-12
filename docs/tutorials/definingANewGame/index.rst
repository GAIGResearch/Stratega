################################
Defining a new game
################################

In this tutorial, we are going to define a game from scratch using Stratega's YAML configuration.
The objective is to create a combat game with two types of units, a fighter and cities.
The fighter can move and attack.
Meanwhile, the city passively produces one gold per turn and can spawn more fighters using the acquired gold.

.. only:: html

   .. figure:: ../../images/tutorialGame.gif

First, we define the type of the game, how much time an agent has per turn, and a limit on how long a game lasts.
Once the round limit is reached, the game will end in a draw.
While we are at it, we define the agents that should play the game.

.. code-block:: yaml

    GameConfig:
        Type: TBS
        ActionTimer: 10s
        RoundLimit: 100

    Agents:
        - RandomAgent
        - HumanAgent

Next, we have to define the Game renderer configuration, with the resolution and the default assets used y the GUI.

.. code-block:: yaml

   GameRenderer:
        Resolution:
            Width: 800
            Height: 600
        Default Assets:
            Selected: ../../assets/Tiles/selected.png
            FogOfWar: ../../assets/Tiles/notVisible.png
  
Next, we define that each player can store gold.
We do this by assigning an additional parameter to the players which represents the currency.

.. code-block:: yaml

    Player:
        Parameters:
            Gold: 5

As the next step, we define the tiles that our map will contain.
Plains are tiles that units can walk on, while water and mountain tiles will block the unit's movement.
To make a tile walkable, we just set the IsWalkable flag to true.
Additionally, we want the mountains to block the view of units to allow for sneaky attacks, which is done by activating the BlocksSight flag.
Finally, don't forget to assign a symbol to each tile since we will later use it to draw the map.

.. code-block:: yaml

    Tiles:
        Plain:
            Sprite: ../../assets/Tiles/plain.png
            Symbol: .
            IsWalkable: true
            DefaultTile: true
        Water:
            Sprite: ../../assets/Tiles/water.png
            Symbol: W
            IsWalkable: false
        Mountain:
            Sprite: .../../assets/Tiles/rock.png
            Symbol: M
            IsWalkable: false
            BlocksSight: true
     
Before we can define the map, we also have to define the entities.
Although buildings, units and vehicles are all quite different, in Stratega, they are all defined as an entity.
Every entity needs a path to the sprite representing it, a symbol for drawing the map, and a value to specify how far it can see.
For this game, we only define two entities.
As with the gold parameter for players, we assign parameters to the entities to represent certain things;
In this case, both entities get a health parameter.
Additionally, we assign the actions and the cost for spawning the fighter, which we will discuss later.

.. code-block:: yaml

    Entities:
        City:
            Sprite: ../../assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            CanSpawn: Fighter
            Actions: [Spawn]
            Parameters:
                Health: 100

        Fighter:
            Sprite: ../../assets/Entities/unit2.png
            Symbol: f
            LineOfSightRange: 6
            Actions: [Move, Attack]
            Parameters:
                Health: 100
            Cost:
                Gold: 3

Now we are done with the tedious definitions, at least for a short moment.
Now we can define the map using the symbols of the tiles and entities we defined earlier.
To do this, draw a rectangular map and you are done. 
To place entities, replace a tile symbol with an entity symbol.
The tile occupied by the entity will always contain the default tile (See the earlier definition of plains).
Note that we can also assign entities to certain players by specifying a number after the entity symbol.
If no number is specified, the entity will be neutral.

.. code-block:: yaml
     
    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  c1 .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  .  .  .  .  M  M  .  M  M
            M  M  .  .  .  .  .  .  W  .  .  .  M  M  .  M  M
            M  M  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
            M  M  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
            M  .  .  .  .  .  W  W  W  W  W  .  .  .  .  .  M
            M  .  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
            M  M  .  .  M  M  .  .  W  .  .  .  .  .  .  M  M
            M  M  .  .  M  M  .  .  .  .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  c0 .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M

Now comes the complex part, defining the actions.
To define an action, we have to specify what it can target and what effects the action will do.

There are many different things an action can target, below you can see a cutout of how to define targets.
For this tutorial, we only need to target positions, entities as well as entity types.
While targeting entities will refer to an existing entity on the board, an entity type references the definition of an entity.
This is useful if we want to spawn something since we can just target an entity type.
For each valid target that Stratega finds, one action will be generated at runtime.
Since we restricted the entity type to only target fighters, there will be at most one action available.
It could also happen that no action is available if the entities player can't afford the fighters cost (See the previous fighter definition) or the entity executing this action cannot spawn fighters (See previous city definition).

The name assigned to each target is later used to specify conditions and effects. In this case, we have one target named "SpawnType".
If we had defined multiple targets, then the number of actions available is equal to the cross-product between all targets.

.. code-block:: yaml

    Targets:
        SpawnType:
            Type: EntityType
            ValidTargets: Fighter
            Conditions:
                - "CanAfford(Source.Player, Target)"
                - "CanSpawn(Source, Target)"

You may have noticed that the target name "SpawnType" indicates that we currently are defining the spawn action for the city.
Below you can see how the full definition of spawn looks like.
It has a cooldown of one tick, which means you can only execute it once per turn in turn-based games.
Whenever an entity executes this action, it will spawn the fighter in a random spot around the source (city) and subtract the fighters cost (3 gold) from the players' gold count.

.. code-block:: yaml

    Spawn:
        Type: EntityAction
        Cooldown: 1
        Targets:
            SpawnType:
                Type: EntityType
                ValidTargets: Fighter
                Conditions:
                    - "CanAfford(Source.Player, SpawnType)"
                    - "CanSpawn(Source, SpawnType)"
        Effects:
            - "SpawnRandom(Source, SpawnType)"
            - "PayCost(Source.Player, SpawnType)"

Next, we have the attack action.
Note that the attack reduces an entities health by explicitly referencing the previously defined parameter.
The effect "Attack" then reduces the parameter by 25 points, and if it falls below 0, removes the attacked entity from the game.
Since Stratega does not prevent you from defining an entity without health, make sure that it can never be targeted by an attack.
Otherwise, the game has to be stopped by Stratega due to an unresolvable error.

.. code-block:: yaml

    Attack:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Target:
                Type: Entity
                ValidTargets: [Fighter, City]
                Conditions: # Only target entities with a distance of 1 to whoever executes this action
                    - "InRange(Source, Target, 1)"
        Effects:
            - "Attack(Target.Health, 25)"

Now we are nearly done. Below you can find the complete list containing all actions, including the move action.
This action does nothing new, except that it targets a position.

.. code-block:: yaml

    Actions:
        Spawn:
            Type: EntityAction
            Cooldown: 1
            Targets:
                SpawnType:
                    Type: EntityType
                    ValidTargets: Fighter
                    Conditions:
                        - "CanAfford(Source.Player, SpawnType)"
                        - "CanSpawn(Source, SpawnType)"
            Effects:
                - "SpawnRandom(Source, SpawnType)"
                - "PayCost(Source.Player, SpawnType)"

        # Attack Actions
        Attack:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: [Fighter, City]
                    Conditions: # Only target entities with a distance of 1 to whoever executes this action
                        - "InRange(Source, Target, 1)"
            Effects:
                - "Attack(Target.Health, 25)"


        # Move Actions
        Move:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Position
                    Shape: Circle
                    Size: 3
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"

The last thing we have to do is define the win- & loss conditions of the game.
We can also assign effects to specific events in the game.
For example, we define the condition that the player who loses his city will lose this game.
Finally, we can specify that each city on the board produces one gold whenever a tick has passed.
A tick in turn-based games is equivalent to all players having played once, or in not overly complicated words: it's our definition of a turn.

.. code-block:: yaml
                   
    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            HasNoCity:
            - "HasNoEntity(Source, City)"

        Trigger:
        - OnTick:
            ValidTargets: City
            Conditions:
                - "IsPlayerEntity(Source)"
            Effects:
                - "ModifyResource(Source.Player.Gold, 1)"


And we are done! Below you can see the full definition of the YAML file.
Feel free to experiment and have a look at the :ref:`YAML-Documentation <yaml-reference>`. to see what else Stratega can do!

++++++++++++++++++++
Full yaml
++++++++++++++++++++

.. code-block:: yaml

    GameConfig:
        Type: TBS
        ActionTimer: 10s
        RoundLimit: 100

    Agents:
        - RandomAgent
        - HumanAgent

    Player:
        Parameters:
            Gold: 5

    Tiles:
        Plain:
            Sprite: ../../assets/Tiles/plain.png
            Symbol: .
            IsWalkable: true
            DefaultTile: true
        Water:
            Sprite: ../../assets/Tiles/water.png
            Symbol: W
            IsWalkable: false
        Mountain:
            Sprite: ../../assets/Tiles/rock.png
            Symbol: M
            IsWalkable: false
            BlocksSight: true
     
    Actions:
        Spawn:
            Type: EntityAction
            Cooldown: 1
            Targets:
                SpawnType:
                    Type: EntityType
                    ValidTargets: Fighter
                    Conditions:
                        - "CanAfford(Source.Player, SpawnType)"
                        - "CanSpawn(Source, SpawnType)"
            Effects:
                - "SpawnRandom(Source, SpawnType)"
                - "PayCost(Source.Player, SpawnType)"

        # Attack Actions
        Attack:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: [Fighter, City]
                    Conditions:
                        - "InRange(Source, Target, 1)"
            Effects:
                - "Attack(Target.Health, 25)"


        # Move Actions
        Move:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Position
                    Shape: Circle
                    Size: 3
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"

    Entities:
        City:
            Sprite: ../../assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            CanSpawn: Fighter
            Actions: [Spawn]
            Parameters:
                Health: 100

        Fighter:
            Sprite: ../../assets/Entities/unit2.png
            Symbol: f
            LineOfSightRange: 6
            Actions: [Move, Attack]
            Parameters:
                Health: 100
            Cost:
                Gold: 3


    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  c1 .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  .  .  .  .  M  M  .  M  M
            M  M  .  .  .  .  .  .  W  .  .  .  M  M  .  M  M
            M  M  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
            M  M  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
            M  .  .  .  .  .  W  W  W  W  W  .  .  .  .  .  M
            M  .  .  .  .  .  .  W  W  W  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  M
            M  M  .  .  M  M  .  .  W  .  .  .  .  .  .  M  M
            M  M  .  .  M  M  .  .  .  .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  c0 .  .  .  .  .  .  M  M
            M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                   
    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            HasNoCity:
            - "HasNoEntity(Source, City)"

        Trigger:
        - OnTick:
            ValidTargets: City
            Conditions:
                - "IsPlayerEntity(Source)"
            Effects:
                - "ModifyResource(Source.Player.Gold, 1)"

