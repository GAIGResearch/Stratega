###############
Protect the Base
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/ProtectTheBase.yaml

.. only:: html

   .. figure:: ../../images/protectTheBaseTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Simple combat TBS game with basic rules. Two players should gather gold and spawn fighters to destroy the enemy base.
Similar to City capturing but with resource gathering.
The gold amount in the mines is limited, take care and dont run off of gold.

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        ActionTimer: 10s
        RoundLimit: 100

    Agents:
        - RandomAgent
        - HumanAgent


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
            BlocksSight: true
        Forest:
            Sprite: ../../GUI/Assets/Tiles/forest.png
            Symbol: F
            IsWalkable: true
     
    Actions:
        # Spawn Actions
        SpawnMiner:
            Type: EntityAction
            Cooldown: 1
            Preconditions:
                - "ResourceGreater(Source.Gold, 50)"
            Targets:
                Target:
                    Type: Position
                    Shape: Square
                    Size: 1
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "SpawnEntity(Source, Miner, Target)"
                - "ModifyResource(Source.Gold, -50)"

        SpawnFighter:
            Type: EntityAction
            Cooldown: 1
            Preconditions:
                - "ResourceGreater(Source.Gold, 100)"
            Targets:
                Target:
                    Type: Position
                    Shape: Square
                    Size: 1
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "SpawnEntity(Source, Fighter, Target)"
                - "ModifyResource(Source.Gold, -100)"

        # Attack Actions
        Attack:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Attackable
                    Conditions:
                        - "InRange(Source, Target, 10)"
            Effects:
                - "Attack(Target.Health, Source.AttackDamage)"

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

        # Resource Actions
        Mine:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: GoldVein
                    Conditions:
                        - "InRange(Source, Target, 1)"
                        - "ResourceGreater(Target.Gold, 40)"
            Effects:
                - "Transfer(Target.Gold, Source.Gold, 40)"

        Deposit:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Base
                    Conditions:
                        - "InRange(Source, Target, 1)"
            Effects:
                - "Transfer(Source.Gold, Target.Gold, Source.Gold)"

    Entities:
        Base:
            Sprite: ../../GUI/Assets/Entities/castle.png
            Symbol: b
            LineOfSightRange: 5
            Actions: [SpawnMiner, SpawnFighter]
            Parameters:
                Gold: 500
                Health: 500

        Miner:
            Sprite: ../../GUI/Assets/Entities/unit7.png
            Symbol: m
            LineOfSightRange: 4
            Actions: [Move, Mine, Deposit]
            Parameters:
                Gold: 0
                Health: 50

        Fighter:
            Sprite: ../../GUI/Assets/Entities/unit2.png
            Symbol: f
            LineOfSightRange: 6
            Parameters:
                AttackDamage: 10
                Health: 80
            Actions: [Move, Attack]

        GoldVein:
            Sprite: ../../GUI/Assets/Entities/gold_chest.png
            Symbol: g
            LineOfSightRange: 6
            Actions: []
            Parameters:
                Gold: 200

    EntityGroups:
        Attackable: [Base, Fighter, Miner]

    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  g  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  g  .  .  .  .  .  W  W  .  .  .  .  .  .  b1 .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  g  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  g  .  .  g  g  g  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  g  .  .  .  .  g  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  .  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  g  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  g  .  .  .  .  .  .  .  .  .  .  .  .  .  b0 .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  M  M  g  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  g  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
                   
    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            HasNoCity:
            - "HasNoEntity(Source, Base)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Gather: [Mine, Deposit]
            Spawn: [SpawnFighter, SpawnMiner]
            Attack: [Attack]