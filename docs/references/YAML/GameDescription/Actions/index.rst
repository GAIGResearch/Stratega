.. _yaml-game-description-actions:

Actions
===========

:Actions: Contains the grouping of actions types into categories. The catogories are defined by default and can be used to know by the agent the type of each action.

:YAML Key: Actions

:Example:
.. code-block:: yaml

    Actions:
        Move: [Move]
        Research: [Research]
        Gather: [Mine]
        Spawn: [Spawn, Build]
        Attack: [Attack]
        Heal: [Heal]

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Move``
     - ``false``
     - ``list``
     - 
     - Assign an Action type to the Move category
   * - ``Research``
     - ``false``
     - ``list``
     - 
     - Assign an Action type to the Research category
   * - ``Gather``
     - ``false``
     - ``list``
     - 
     - Assign an Action type to the Gather category
   * - ``Spawn``
     - ``false``
     - ``list``
     - 
     - Assign an Action type to the Spawn category
   * - ``Attack``
     - ``false``
     - ``list``
     - 
     - Assign an Action type to the Attack category
   * - ``Heal``
     - ``false``
     - ``list``
     - 
     - Assign an Action type to the Heal category
