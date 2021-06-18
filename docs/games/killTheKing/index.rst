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
        Forest:
            Sprite: ../../GUI/Assets/Tiles/forest.png
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
                    Conditions:
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
                    Shape: Square
                    Size: 1
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
                    Conditions:
                        - "InRange(Source, Target, Source.HealRange)"
            Effects:
                - "ModifyResource(Target.Health, Source.HealAmount)"


    Entities:
        Warrior:
            Sprite: ../../GUI/Assets/Entities/unit2.png
            Symbol: w
            LineOfSightRange: 6
            Actions: [Attack, Move]
            Parameters:
                Health: 200
                AttackRange: 2
                AttackDamage: 100
                MovementPoints: 2

        Archer:
            Sprite: ../../GUI/Assets/Entities/unit4.png
            Symbol: a
            LineOfSightRange: 10
            Parameters:
                Health: 100
                AttackRange: 5
                AttackDamage: 10
                MovementPoints: 2
            Actions: [Attack, Move]

        Healer:
            Sprite: ../../GUI/Assets/Entities/unit3.png
            Symbol: h
            LineOfSightRange: 4
            Parameters:
                Health: 40
                HealRange: 2
                HealAmount: 10
                MovementPoints: 5
            Actions: [Heal, Move]

        King:
            Sprite: ../../GUI/Assets/Entities/unit1.png
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