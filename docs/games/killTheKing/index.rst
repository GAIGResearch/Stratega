###############
Kill the king
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/KillTheKing.yaml

.. only:: html

   .. figure:: ../../images/killTheKingTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Simple combat TBS game with basic rules. Two players should fight to kill the enemy king.
In this game the player has a fixed number of entities and cant spawn anymore. Use your warriors and archer to attack the enemies, each player has one healer and one king.
Choose your strategy wisely, would you attack his king or defend yours?

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        RoundLimit: 100
        FogOfWar: true

    Agents:
        - HumanAgent
        - OSLAAgent
        #- MCTSAgent:
        #    FmCalls: 100
        #    Budget: TIME
        #    RolloutLength: 10

    GameRenderer:
        Resolution:
            Width: 800
            Height: 600
        Default Assets:
            Selected: ../../assets/Tiles/selected.png
            FogOfWar: ../../assets/Tiles/notVisible.png
        #Optional:
        #Font: ../../assets/arial.ttf
        #OutlineShader: ../../assets/OutLine.frag

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
        Forest:
            Sprite: ../../assets/Tiles/forest.png
            Symbol: F
            IsWalkable: true

            
    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  a1 .  .  .  .  .  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  a1 k1 h1 .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  .  .  w1 w1 .  .  .  .  W  W  .  .  .  .  .  .  .  .  .  .  .  M  M  .  .  .  .  .  M  M  M
            M  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  W  W  W  W  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W
            M  .  .  .  .  .  .  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  .  W  W  W  W  W
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  W  W  W  W  W
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  a0 .  .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  a0 k0 h0 .  .  .  .  .  M
            M  M  M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  w0 w0 .  .  .  .  .  M
            M  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M

    Actions:
        Attack:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: All
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: AllPositions
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
                        - "InRange(Source, Target, Source.AttackRange)"
            Effects:
                - "Attack(Target.Health, Source.AttackDamage)"

        Move:
            Type: EntityAction
            Preconditions:
                - "ResourceGreaterEqual(Source.MovementPoints, 1)"
            Targets:
                Target:
                    Type: Position
                    SamplingMethod: 
                        Type: Dijkstra
                        Options:
                            SearchSize: 3
                            AllowDiagonals: false
                    #    Type: Neighbours
                    #    Options:
                    #        Shape: Circle
                    #        Size: 1
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"
                - "ModifyResource(Source.MovementPoints, -1)"

        Heal:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: All
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: AllPositions
                    Conditions:
                        - "InRange(Source, Target, Source.HealRange)"
            Effects:
                - "ModifyResource(Target.Health, Source.HealAmount)"


    Entities:
        Warrior:
            Sprite: ../../assets/Entities/unit_2.png
            Symbol: w
            LineOfSightRange: 6
            Actions: [Attack, Move]
            Parameters:
                Health: 200
                AttackRange: 2
                AttackDamage: 100
                MovementPoints: 2

        Archer:
            Sprite: ../../assets/Entities/unit_3.png
            Symbol: a
            LineOfSightRange: 10
            Parameters:
                Health: 100
                AttackRange: 5
                AttackDamage: 10
                MovementPoints: 2
            Actions: [Attack, Move]

        Healer:
            Sprite: ../../assets/Entities/unit_6.png
            Symbol: h
            LineOfSightRange: 4
            Parameters:
                Health: 40
                HealRange: 2
                HealAmount: 10
                MovementPoints: 5
            Actions: [Heal, Move]

        King:
            Sprite: ../../assets/Entities/unit_1.png
            Symbol: k
            LineOfSightRange: 6
            Parameters:
                Health: 400
                AttackRange: 2
                AttackDamage: 100
                MovementPoints: 1
            Actions: [Attack, Move]

    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            NoHasKing:
            - "HasNoEntity(Source, King)"

        Trigger:
            - OnTick:
                Effects:
                    - "SetToMaximum(Source.MovementPoints)"

    #Action categories
    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Heal: [Heal]
            Attack: [Attack]
        Entities:
            Unit: [King, Warrior, Archer, Healer]
            Fighter: [Warrior, Archer]
            NoFighter: [Healer]
            Melee: [Warrior]
            Ranged: [Archer]

    GameRunner:
        AgentInitializationTime:
            Enabled: false
            BudgetTimeMs: 50
            DisqualificationTimeMs: 70
        AgentComputationTime:
            Enabled: false
            BudgetTimeMs: 1000
            DisqualificationTimeMs: 70
            MaxNumberWarnings: 5