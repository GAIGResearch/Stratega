.. _yaml-winconditions:

Forward Model
===========

:Forward Model: Contais the definition of the forwardmodel.
:YAML Key: ForwardModel

:Example:
.. code-block:: yaml

    ForwardModel:
        WinConditions: #If true: Player -> win game
            UnitAlive:
            - "HasEntity(Source, Wonder)"

        LoseConditions: #If true: Player -> cant play
            HasResource:
            - "ResourceLowerEqual(Source.Food, 0)"

        Trigger:
        #Global resource compsumption
            - OnTick:
                Type: Entity
                ValidTargets: Buildings
                Conditions:
                    - "IsPlayerEntity(Source)"
                Effects:
                    - "ModifyResource(Source.Player.Food, -1)"

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`WinConditions <yaml-forwardmodel-win-conditions>`
     - ``false``
     - ``map``
     - 
     - Defines the list of conditions to win the game.
   * - :ref:`LoseConditions <yaml-forwardmodel-lose-conditions>`
     - ``false``
     - ``map``
     - 
     - Defines the list of conditions to lose the game.
   * - :ref:`Trigger <yaml-forwardmodel-trigger>`
     - ``false``
     - ``map``
     - 
     - Defines the list of on tick actions.