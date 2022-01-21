.. _yaml-forwardmodel-lose-conditions:

LoseConditions
===========

:Lose Conditions: Contais the list of conditions to lose the game.

:YAML Key: LoseConditions

:Example:
.. code-block:: yaml

    LoseConditions: #If true: Player -> cant play
        HasResource:
        - "ResourceLowerEqual(Source.Food, 0)"

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Win condition name``
     - ``false``
     - ``list``
     - 
     - If this condition returns true the player will not be able to play the game.
