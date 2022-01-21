.. _yaml-targets:

Targets
===========

:Targets: Contains list of targets of a action.

:YAML Key: Targets

:Example:
.. code-block:: yaml

    Targets:
        EntityTypeTarget:
            Type: EntityType
            ValidTargets: Wonder
            Conditions:
                - "CanAfford(Source.Player, EntityTypeTarget)"
                - "CanSpawn(Source, EntityTypeTarget)"

        TargetPosition:
            Type: Position
            SamplingMethod: 
                Type: Neighbours
                Options:
                    Shape: Square
                    Size: 4
            Conditions:
                - "IsWalkable(TargetPosition)"


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`Target <yaml-target>`
     - ``true``
     - ``map``
     - 
     - Define the a target