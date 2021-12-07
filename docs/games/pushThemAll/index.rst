###############
Push them all
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/PushThemAll.yaml

.. only:: html

   .. figure:: ../../images/pushThemAllTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

TBS game in which two players starts with a fixed amount of warriors, the are only two actions: Move and Push. Dont fall in the holes and push the enemy into them to kill the unit.

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        RoundLimit: 1000
        FogOfWar: false

    Buffs: # Buff lists
        AttackBoostBuff:
            AdditiveModifier: 
                AttackDamage: 20

    Agents:
        - HumanAgent
        - PusherAgent

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
        Hole:
            Sprite: ../../assets/Tiles/hole.png
            Symbol: H
            IsWalkable: true
            
    Board:
        GenerationType: Manual
        Layout: |-
            M  M  M  M  H  M  M  M  M
            M  H  H  H  H  H  H  H  M
            M  H  .  w1 .  w1 .  H  M
            M  H  .  w1 .  w1 .  H  M
            M  H  .  .  .  .  .  H  M
            M  H  H  .  H  .  H  H  M
            M  H  .  .  .  .  .  H  M
            M  H  .  H  .  H  .  H  M
            M  H  .  H  .  H  .  H  M
            M  H  .  .  .  .  .  H  M
            M  H  H  .  H  .  H  H  M
            M  H  .  .  .  .  .  H  M
            M  H  .  w0 .  w0 .  H  M
            M  H  .  w0 .  w0 .  H  M
            M  H  H  H  H  H  H  H  M
            M  M  M  M  H  M  M  M  M
    Actions:
        Push:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Warrior
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 1
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
            Effects:
                - "Push(Source, Target)"


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
                            SearchSize: 1
                            AllowDiagonals: false
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "Move(Source, Target)"
                - "ModifyResource(Source.MovementPoints, -1)"

    Entities:
        Warrior:
            Sprite: ../../assets/Entities/unit_2.png
            Symbol: w
            LineOfSightRange: 6
            Actions: [Push, Move]
            Parameters:
                Health: 200
                AttackRange: 2
                AttackDamage: 100
                MovementPoints: 1       

    ForwardModel:
        LoseConditions: #If true: Player -> cant play
            HasNoCity:
            - "HasNoEntity(Source, Warrior)"
        Trigger:
            - OnTick:
                Type: Entity
                ValidTargets: Warrior
                Effects:
                    - "SetToMaximum(Source.MovementPoints)"
            - OnAdvance:
                Type: Entity
                ValidTargets: Warrior
                Conditions:
                    - "IsTileType(Source, Hole)"
                Effects:
                    - "Remove(Source)"


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