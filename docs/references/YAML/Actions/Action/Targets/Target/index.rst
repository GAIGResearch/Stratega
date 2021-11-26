.. _yaml-target:

Target
===========

:Targets: Contais the definition of a target

:YAML Key: Target

:Example:
.. code-block:: yaml

    TargetPosition:
        Type: Position
        ValidTargets: All
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
   * - ``Type``
     - ``true``
     - ``type``
     - Position\EntityType\Entity\Technology\ContinuousAction
     - Define the type of the target
   * - :ref:`SamplingMethod <yaml-sampling-method>`
     - ``false``
     - ``type``
     -
     - Define the sampling method to sample the targets.
   * - ``ValidTargets``
     - ``false``
     - ``list\single value\entity group``
     - 
     - List of conditions to check if the target is valid.
   * - ``Conditions``
     - ``false``
     - ``list``
     - 
     - List of conditions to check if the target is valid.