#############
Configuration
#############

In this section, we will have a closer look on the YAML configuration file.
It allows users to adjust the balance of an existing game or define new game modes by simply modifying the game-components through keywords.

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
            Sprite: ../../GUI/Assets/Tiles/plain.png
            Symbol: .
            IsWalkable: true
            DefaultTile: true
        Water:
            Sprite: ../../GUI/Assets/Tiles/water.png
            Symbol: W
            IsWalkable: false
        Mountain:
            Sprite: ../../GUI/Assets/Tiles/rock.png
            Symbol: M
            IsWalkable: false
        Forest:
            Sprite: ../../GUI/Assets/Tiles/forest.png
            Symbol: F
            IsWalkable: true

    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  c1 .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
            M  .  .  c  .  .  W  W  c  W  W  .  .  c  .  .  M
            M  .  .  .  .  .  .  W  .  W  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  c0 .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                   


First, we will take a loot at the definition of tiles and board.
Similar to the level description in the video game definition language, we first define tiles and their encoding in the tilemap.
Each tile definition consists of a symbol, a sprite, if it is walkable and if it is blocking the view.
Further tile attributes will be added during in upcoming versions of the framework.

Later on, the board can be manually described in terms of a tile-map, an $n$-by-$m$ grid of tiles.
Alternatively, it will be possible to procedurally generate levels.
Therefore, the user needs to provide a method which will receive the tile configuration and a set of parameters to generate a level.
This allows the framework to be used for research on level generation.

.. code-block:: yaml

    Entities:
        City:
            Sprite: ../../GUI/Assets/Entities/castle.png
            Symbol: c
            LineOfSightRange: 5
            CanSpawn: Units
            Actions: [Spawn]

        Conquerer:
            Sprite: ../../GUI/Assets/Entities/unit7.png
            Symbol: s
            LineOfSightRange: 4
            Actions: [Move, Capture]
            Parameters:
                Health: 25
            Cost:
                Gold: 6

        Fighter:
            Sprite: ../../GUI/Assets/Entities/unit2.png
            Symbol: f
            LineOfSightRange: 6
            Actions: [Move, Attack]
            Parameters:
                Health: 100
            Cost:
                Gold: 5

    EntityGroups:
        Units: [Fighter, Conquerer]


Next, entities are defined in terms of their type and their properties.
In contrast to previous versions of the framework, the unit definition can use an arbitrary set of parameters.
Parameters will be used by actions as we will see in the next paragraph.
Typical parameters include health, movement, and damage.
In case of partial observable game, each entity requires to set the line of sight range.
Here, each entity can observe tiles with a Euclidean distance equal or less than the given threshold.
A player's view of the board will be limited by the union of all tiles visible by their controlled entities.
Finally, each entity receives a list of actions.

The configuration shows the definition of a City, a Fighter and a Conquerer.
Note, that they implement a different set of parameters and actions.
The action definitions are separated into another section to make the same action reusable across entity types.

.. code-block:: yaml
     
    Actions:
        Spawn:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: EntityType
                    ValidTargets: Units
                    Conditions:
                        - "CanAfford(Source.Player, Target)"
                        - "CanSpawn(Source, Target)"
            Effects:
                - "SpawnRandom(Source, Target)"
                - "PayCost(Source.Player, Target)"

        # Attack Actions
        Attack:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: [Conquerer, Fighter]
                    Conditions:
                        - "InRange(Source, Target, 1)"
            Effects:
                - "Attack(Target.Health, 25)"

        # Capturing
        Capture:
            Type: EntityAction
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: City
                    Conditions:
                        - "InRange(Source, Target, 1)"
            Effects:
                - "ChangeOwner(Target, Source.Player)"
                - "Remove(Source)"

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


The next part shows the definition of the special actions Attack, Capture, Spawnrandom and Move. An actions type shows if it will be applied to players, map tiles, or entities. A cooldown can be used in case the action should not be available every turn.
The keywords Conditions and Effects show how parameters of the executing entity can be used to implement the wished effect.
Some baseline effects such as Attack and ModifyRessources simplify the implementation of typical game-play mechanics.


.. code-block:: yaml
                   
    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            NoHasCity:
            - "NoHasEntity(Source, City)"

        Trigger:
            - OnTick:
                ValidTargets: City
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Gold, 1)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Spawn: [Spawn]
            Attack: [Attack, Capture]


The next part of our configuration consists of the forward model's properties.
Here, we first defined a lose condition.
In this case, we have only one lose condition called NoHasCity. It use the same conditions than the actions, we only have one that will check if the player has not a city.
With the Trigger and Effects keywords users can introduce more general game mechanics.
An effect's trigger describes when the effect will be executed.
Conditions can further limit the effects to be only applied to conforming units or tiles.
Here, the number of movement points are replenished every turn.

In upcoming versions of our framework, we will extend the set of available keywords to ease the development of diverse game-modes.

.. code-block:: yaml

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Spawn: [Spawn]
            Attack: [Attack, Capture]

The last part shows the game description, it can be used by the agent to query actions types grouped in different categories.
Due to the high flexibility of Stratega and the unlimited definition of Actions, we realized that agents would need a way to search actions by simple categories.