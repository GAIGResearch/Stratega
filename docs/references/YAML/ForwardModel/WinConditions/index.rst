.. _yaml-forwardmodel-win-conditions:

WinConditions
===========

:Win Conditions: Contais the list of conditions to win the game.

:YAML Key: WinConditions

:Example:
.. code-block:: yaml

    WinConditions: #If true: Player -> win game
        UnitAlive:
        - "HasEntity(Source, Wonder)"
        #Define here any condition to win the game

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
     - If this condition returns true the player will win the game.
