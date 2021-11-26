.. _yaml-game-description-entities:

Entities
===========

:Entities: Contains the grouping of entities types into categories. The catogories are defined by default and can be used to know by the agent the type of each entity.

:YAML Key: Entities

:Example:
.. code-block:: yaml

    Entities:
        Base: [City]
        Building: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage]
        Spawner: [City, Barracks, MilitaryAcademy]
        Unit: [Worker, Warrior, Archer, Catapult]
        Fighter: [Warrior, Archer, Catapult]
        NoFighter: [Worker]
        Melee: [Warrior]
        Ranged: [Archer, Catapult]

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Base``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Base category
   * - ``Building``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Building category
   * - ``Spawner``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Spawner category
   * - ``Unit``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Unit category
   * - ``Fighter``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Fighter category
   * - ``NoFighter``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the NoFighter category
   * - ``Melee``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Melee category
   * - ``Ranged``
     - ``false``
     - ``list``
     - 
     - Assign an Entity type to the Ranged category
