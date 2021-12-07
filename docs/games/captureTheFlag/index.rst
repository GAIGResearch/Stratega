###############
CaptureTheFlag
###############

Yaml paths:

.. code-block:: c++

    /gameConfigs/TBS/CaptureTheFlag.yaml

.. only:: html

   .. figure:: ../../images/captureTheFlagTBS.gif

++++++++++++++++++++
Description
++++++++++++++++++++

Every ten 10 ticks a flag will be spwaned if there is not any flag in the board already. Your mission is capture it and delivery it to your base. Take care, because your movement point will be lower while you carry the flag.

++++++++++++++++++++
YAML
++++++++++++++++++++
.. code-block:: yaml

    GameConfig:
        Type: TBS
        RoundLimit: 1000
        FogOfWar: true
        Parameters:
            FlagsCaptured: [0,0,10]
            CurrentSpawnedFlagsNumber: [0,0,5]

    Buffs: # Buff lists
        SlowBuff:
            MultiplicationModifier:
                MovementPoints: 0.5

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
            SpawnedUnits: [0,1,10]
            Flags: [0,0,100]
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
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
            M  .  .  .  .  .  M  M  W  W  W  W  W  W  W  W  M
            M  .  b1 .  .  .  .  M  .  .  .  .  .  .  .  W  M
            M  .  .  w1 .  .  .  .  .  .  .  .  .  .  .  W  M
            M  .  .  .  .  .  .  .  .  .  .  .  W  W  .  W  M
            M  .  .  .  .  .  W  W  .  .  .  .  W  W  .  W  M
            M  M  .  .  .  .  W  W  .  .  .  .  .  .  .  W  M
            M  M  M  M  .  .  .  W  W  W  .  .  .  .  .  W  M
            M  W  M  .  .  .  .  W  W  W  W  .  .  .  M  W  M
            M  W  .  .  .  .  .  .  .  W  W  W  .  .  M  M  M
            M  W  .  .  .  .  .  .  .  .  W  W  .  .  .  M  M
            M  W  .  .  W  W  .  .  .  .  .  .  .  .  .  .  M
            M  W  .  .  W  W  .  .  .  .  .  .  .  .  .  .  M
            M  W  .  .  .  .  .  .  .  M  .  .  .  w0 .  .  M
            M  W  .  .  .  .  .  .  M  M  .  .  .  .  b0 .  M
            M  W  W  W  W  W  W  W  W  M  .  .  .  .  .  .  M
            M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M  M
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

        Capture:
            Type: EntityAction
            Preconditions:
                - "ResourceLowerEqual(Source.Flags, 0)"
            Targets:
                Target:
                    Type: Entity
                    SamplingMethod: 
                        Type: Dijkstra
                        Options:
                            SearchSize: 1
                            AllowDiagonals: false
                    #    Type: Neighbours
                    #    Options:
                    #        Shape: Circle
                    #        Size: 1
                    ValidTargets: Flag
            Effects:
                - "ModifyResource(GameState.CurrentSpawnedFlagsNumber, -1)"
                - "ModifyResource(Source.Flags, +1)"
                - "Remove(Target)"

        Deposit:
            Type: EntityAction
            Cooldown: 1
            Targets:
                Target:
                    Type: Entity
                    ValidTargets: Base
                    SamplingMethod: 
                        Type: Neighbours
                        Options:
                            Shape: Square
                            Size: 1
            Effects:
                - "Transfer(Source.Flags, Target.Player.Flags, Source.Flags)"


    Entities:
        Warrior:
            Sprite: ../../assets/Entities/unit_2.png
            Symbol: w
            LineOfSightRange: 6
            Actions: [Attack, Move, Capture, Deposit]
            Parameters:
                Health: 200
                AttackRange: 2
                AttackDamage: 100
                MovementPoints: 2
                Flags: [0,0,1]
        Flag:
            Sprite: ../../assets/Entities/wonder.png
            Symbol: f
            LineOfSightRange: 5

        Base:
            Sprite: ../../assets/Entities/castle.png
            Symbol: b
            LineOfSightRange: 5
            Actions: [SpawnUnit]
                


    ForwardModel:
        WinConditions: #If true: Player -> win game
            Flags:
            - "ResourceGreaterEqual(Source.Flags, 5)"

        Trigger:
            - OnTick:
                Type: Entity
                ValidTargets: [Warrior]
                Effects:
                    - "SetToMaximum(Source.MovementPoints)"
            - OnTick:
                Type: Entity
                ValidTargets: [Warrior]
                Conditions:
                    - "ResourceGreaterEqual(Source.Flags, 1)"
                    - "HasNoBuff(Source, SlowBuff)"
                Effects:
                    - "ApplyBuff(Source, SlowBuff, 1)"
            - OnTick:
                #Type: Entity, Player, State
                Type: GameState
                Conditions:
                    - "IsTickMultipleOf(10)"
                    - "ResourceLowerEqual(Source.CurrentSpawnedFlagsNumber, 0)"
                Effects:
                    - "SpawnEntityRandomLocation(Flag)"
                    - "ModifyResource(Source.CurrentSpawnedFlagsNumber, +1)"


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