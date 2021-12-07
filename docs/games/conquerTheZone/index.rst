###############
Conquer the Zone
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/ConquerTheZone.yaml

.. only:: html

   .. figure:: ../../images/conquerTheZoneTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

TBS game in which two player fight for the capture of the zones. The player with more units around a zone will take ownership of it.
The more zones you have the more points you earn each tick.

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        RoundLimit: 10000
        FogOfWar: true

    Buffs: # Buff lists
        AttackBoostBuff:
            AdditiveModifier: 
                AttackDamage: 20

    Agents:
        - HumanAgent
        - DoNothingAgent

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

    Player:
        Parameters:
            Zones: [0,0,5]
            SpawnedUnits: [0,0,5]
            Score: [0,0,200]
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
            M  M  M  M  M  M  M  M
            M  .  .  .  W  W  W  M
            M  .  b1 .  .  .  W  M
            M  .  .  w1 .  .  W  M
            M  .  .  .  .  .  W  M
            M  .  .  .  .  .  W  M
            M  M  .  .  .  .  W  M
            M  M  M  M  .  .  W  M
            M  W  M  .  .  M  W  M
            M  W  .  .  .  M  M  M
            M  W  .  .  .  .  M  M
            M  M  .  .  .  z  W  M
            M  M  M  M  .  .  W  M
            M  W  M  .  .  M  W  M
            M  W  .  .  .  M  M  M
            M  W  z  .  .  .  M  M
            M  M  .  .  .  .  W  M
            M  M  M  M  .  .  W  M
            M  W  M  .  .  M  W  M
            M  W  .  .  .  M  M  M
            M  W  .  .  .  .  M  M
            M  M  .  .  .  z  W  M
            M  M  M  M  .  .  W  M
            M  W  M  .  .  M  W  M
            M  W  .  .  .  M  M  M
            M  W  .  .  .  .  M  M
            M  W  .  .  .  .  .  M
            M  W  .  .  .  .  .  M
            M  W  .  .  w0 .  .  M
            M  W  .  .  .  b0 .  M
            M  W  W  W  .  .  .  M
            M  M  M  M  M  M  M  M
    Actions:
        SpawnUnit:
            Type: EntityAction
            Cooldown: 1
            Preconditions:
                - "ResourceLowerEqual(Source.Player.SpawnedUnits, 4)"
            Targets:
                Target:
                    Type: Position
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Circle
                            Size: 3
                    Conditions:
                        - "IsWalkable(Target)"
            Effects:
                - "SpawnEntity(Source, Warrior, Target)"
                - "ModifyResource(Source.Player.SpawnedUnits, +1)"

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
                            SearchSize: 5
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

        CheckPlayer:
            Type: EntityAction
            Targets:
                Target:
                    Type: Entity
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 1
                    ValidTargets: Warrior
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
            Effects:
                - "ModifyResource(Source.Control, +1)"

        CheckPlayerOwn:
            Type: EntityAction
            Targets:
                Target:
                    Type: Entity
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 1
                    ValidTargets: Warrior
                    Conditions:
                        - "SamePlayer(Source, Target)"
            Effects:
                - "ModifyResource(Source.Control, -1)"

        CheckControl:
            Type: EntityAction        
            Preconditions:
                - "ResourceGreaterEqual(Source.Control, 10)"
            Targets:
                Target:
                    Type: Entity
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 1
                    ValidTargets: Warrior
                    Conditions:
                        - "DifferentPlayer(Source, Target)"
            Effects:
                - "ChangeOwner(Source, Target.Player)"
                - "ChangeResource(Source.Control, 0)"

        BoostAttackPower:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: [Warrior]
                    SamplingMethod:
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 2
                    Conditions:
                        - "HasNoBuff(Target, AttackBoostBuff)"
                        - "SamePlayer(Source, Target)"
            Effects:
                - "ApplyBuff(Target, AttackBoostBuff, 5)"

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
        Zone:
            Sprite: ../../assets/Entities/wonder.png
            Symbol: z
            LineOfSightRange: 5
            OnTickActions: [CheckPlayer,CheckPlayerOwn , CheckControl, BoostAttackPower]
            Parameters:
                Control: [0,0,10]

        Base:
            Sprite: ../../assets/Entities/castle.png
            Symbol: b
            LineOfSightRange: 5
            Actions: [SpawnUnit]            


    ForwardModel:
        WinConditions: #If true: Player -> win game
            Flags:
            - "ResourceGreaterEqual(Source.Score, 200)"

        Trigger:
            - OnTick:
                Type: Entity
                ValidTargets: [Warrior]
                Effects:
                    - "SetToMaximum(Source.MovementPoints)"
            - OnTick:
                Type: Player
                Effects:
                    - "ChangeResource(Source.Zones, 0)"
            - OnTick:
                Type: Entity
                ValidTargets: [Zone]
                Conditions:
                    - "IsNotNeutral(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Zones, +1)"
                    - "ModifyResource(Source.Player.Score, +1)"


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