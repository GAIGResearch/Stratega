.. _yaml-actions:

Actions
===========

:Actions: Contains the definition of the actions used in the game.
:YAML Key: Actions

:Example:
.. code-block:: yaml

    Actions:
        # Attack Actions
        Attack:
            Type: EntityAction
            Cooldown: 1
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


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`Action <yaml-action>`
     - ``false``
     - ``map``
     - 
     - Action type definition
