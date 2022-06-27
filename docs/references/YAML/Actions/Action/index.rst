.. _yaml-action:

Action
===========

:Action: Contais the definition the action type.

:YAML Key: Action

:Example:
.. code-block:: yaml

    Attack:
        Type: EntityAction
        Cooldown: 1
        Preconditions:
            - "ResourceGreaterEqual(Source.AttackPoints, 1)"
        Targets:
            Target:
                Type: Entity
                ValidTargets: Attackable
                SamplingMethod:
                    Type: Neighbours
                    Options:
                        Shape: AllPositions
                Conditions:
                    - "DifferentPlayer(Source, Target)"
                    - "InRange(Source, Target, Source.Range)"
        Effects:
            - "Attack(Target.Health, Source.WeaponStrength)"

    #Continuous action

    Research:
        Type: PlayerAction
        Cooldown: 0
        Targets:
            Target:
                Type: Technology
                ValidTargets: All
                Conditions:
                    - "CanResearch(Source, Target)"
                    - "CanAfford(Source, Target)"
        TriggerComplete:
            - "HasElapsedTick(Target.Time)"
        OnStart:
            - "PayCost(Source, Target)"
        OnTick:
            - "PayCost(Source, Target)"
        OnComplete:
            - "Research(Source, Target)"

    #Inbuilt effects Sample effect/ random effect and conditional effect
    #Check predefined game TBS/Tests/KillTheKingActions.yaml for more information
    Attack:
        Type: EntityAction
        Cooldown: 1
        Targets:
            Position:
                Type: Position
                SamplingMethod:
                    Type: Neighbours
                    Options:
                        Shape: AllPositions
                Conditions:
                    #- "DifferentPlayer(Source, Target)"
                    - "InRange(Source, Position, Source.AttackRange)"
        Effects:
            SampleEffect: #Single effect with sampled entities
                Targets:
                    Targettwo:
                        Type: Entity
                        ValidTargets: All
                        SamplingMethod:
                            Type: Neighbours
                            Options:
                                Shape: AllPositions
                        Conditions:
                            - "InRange(Position, Targettwo, Source.AttackRange)"
                Effects:
                    #- "Attack(Targettwo.Health, Source.AttackDamage)"
                    RandomEffect: #Set of effects associated to a probability
                        0.1: 
                            - "Attack(Targettwo.Health, Source.AttackDamage)"
                        0.9: 
                            - "Attack(Targettwo.Health, Source.AttackDamage)"
                    ConditionalEffect: #Set of effects associated to a conditions:
                        Conditions:
                            - "DifferentPlayer(Source, Targettwo)"
                            #- "InRange(Position, Targettwo, Source.AttackRange)"
                        Effects:
                            - "Attack(Targettwo.Health, Source.AttackDamage)"


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Type``
     - ``true``
     - ``type``
     - PlayerAction/EntityAction
     - Define the action source type 
   * - ``Cooldown``
     - ``false``
     - ``int``
     - 
     - Define the ticks cooldown to use this action by the source.
   * - ``Targets``
     - ``true``
     - ``map``
     - 
     - List of targets.
   * - :ref:`Targets <yaml-targets>`
     - ``false``
     - ``list``
     - 
     - List of actions this entity can generate.
   * - ``Preconditions``
     - ``false``
     - ``list``
     - 
     - List of preconditions executed before checking any target.
   * - ``Effects``
     - ``false``
     - ``list``
     - 
     - List of effects executed in the action execution.
   * - ``TriggerComplete / Continuous Action``
     - ``false``
     - ``list``
     - 
     - List of conditions to check if the Continuous actions is completed, is checked every tick.
   * - ``OnStart / Continuous Action``
     - ``false``
     - ``list``
     - 
     - List of effects executed at the intial execution of the action.
   * - ``OnTick / Continuous Action``
     - ``false``
     - ``list``
     - 
     - List of effects executed each tick.
   * - ``OnComplete / Continuous Action``
     - ``false``
     - ``list``
     - 
     - List of effects executed after the action is completed.
   * - ``OnAbort / Continuous Action``
     - ``false``
     - ``list``
     - 
     - List of effects executed if the Continuous action is aborted.


Optionally there is a list of effects that can be used instead the ones defined in effects factory.

- Sample effect: Sample effects with specific targets.
- Random effect: Execute effects associated to a probability.
- conditional effect: Execute effects associated to a condition.

