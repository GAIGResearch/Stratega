.. _yaml-forwardmodel-trigger:

Trigger
===========

:Trigger: Contais the list of triggers. There are three types of triggers: OnTick, executed each tick; OnAdvance, executed each AdvanceGameState and OnSpawn, executed each entity spawn.

:YAML Key: Trigger

:Example:
.. code-block:: yaml

    Trigger:
    #Global resource compsumption
        - OnTick:
            Type: Entity
            ValidTargets: Buildings
            Conditions:
                - "IsPlayerEntity(Source)"
            Effects:
                - "ModifyResource(Source.Player.Food, -1)"
        - OnTick:
            Type: Entity
            ValidTargets: Units
            Conditions:
                - "IsPlayerEntity(Source)"
            Effects:
                - "ModifyResource(Source.Player.Food, -1)"
        - OnTick:
            #Type: Entity, Player, State
            Type: GameState
            Conditions:
                - "IsTickMultipleOf(10)"
                - "ResourceLowerEqual(Source.CurrentSpawnedFlagsNumber, 0)"
            Effects:
                - "SpawnEntityRandomLocation(Flag)"
                - "ModifyResource(Source.CurrentSpawnedFlagsNumber, +1)"
        - OnAdvance:
            Type: Entity
            ValidTargets: Warrior
            Conditions:
                - "IsTileType(Source, Hole)"
            Effects:
                - "Remove(Source)"

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Type``
     - ``true``
     - ``SourceType``
     - Entity/GameState/Player
     - Define the source of the action.
   * - ``ValidTargets``
     - ``true``
     - ``bool``
     -  true/false
     - List of valid targets affected by this OnTick action.
   * - ``Conditions``
     - ``true``
     - ``int``
     - 
     - Defines the list of conditions. If all the condition returns true the effects will be executed.
   * - ``Effects``
     - ``true``
     - ``int``
     - 
     - Define the list of effects executed on each target.