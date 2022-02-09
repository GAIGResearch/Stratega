.. _yaml-entity:

Entity
===========

:Entity: Contais the definition of the entity type.

:YAML Key: Entity

:Example:
.. code-block:: yaml

    City:
        Sprite: ../../assets/Entities/castle.png
        Symbol: c
        LineOfSightRange: 5
        Actions: [Spawn]
        CanSpawn: [Worker]
        Cost:
            Prod: 10
        Parameters:
            Health: 200
            StorageCapacity: 50
            Range: 6
        OnTickActions: [AddBuffAround]

    #Object definitions have more properties
    Key:
       Sprite: ../../assets/Entities/key.png
       Symbol: k
       CanEquip: [Searcher] #Who can equip the object in the slots
       SlotsUse: [Right hand] #empty == cant be equiped in any slot

       #Source object , Target entity
       #OnEquip: 
           #Conditions: #conditions executed when the object is added to a slot
           #Effects: #effects executed when the object is added to a slot
       #OnAddedInventory: 
           #Conditions: #conditions executed when the object is added to the inventory
           #Effects: #effects executed when the object is added to the inventory
       OnUseSlot: 
           #Conditions: #conditions executed when the object is used in the slot
           Effects: #effects executed when the object is used in the slot
               - "ModifyResource(Target.Player.DoorsOpened, 1)"
       #OnUseInventory:
           #Conditions: #effects executed when the object is used in the inventory
           #Effects: #effects executed when the object is used in the inventory
           #    - "ModifyResource(Target.Health, Source.HealAmount)"

       #OnTickActions: [] #Execute when in on slot or inventory
       #Parameters:
       #    HealAmount: 20
       #    HealRange: 2

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Sprite``
     - ``true``
     - ``string``
     -  
     - Path to the sprite used to render the entity in the GUI mode.
   * - ``Symbol``
     - ``true``
     - ``char``
     - 
     - Symbol used to parse the map board.
   * - ``LineOfSightRange``
     - ``true``
     - ``int``
     - 
     - Line of sight of the unit. Used to render the fog of war.
   * - ``Actions``
     - ``false``
     - ``list``
     - 
     - List of actions this entity can generate.
   * - ``CanSpawn``
     - ``false``
     - ``list``
     - 
     - List of entities this entity can spawn.
   * - ``Cost``
     - ``false``
     - ``map``
     - 
     - Map of parameter to cost, used to know the price to spawn this entity.
   * - ``Parameters``
     - ``false``
     - ``map``
     - 
     - List of parameters of the entity. Optionally you can define the min and max value of the parameter by defining it with the following template "[min,default,max]".
   * - ``CanEquip``
     - ``false``
     - ``list``
     - 
     - List of entities who can equip this object.
   * - ``Slots use``
     - ``false``
     - ``list``
     - 
     - List of slots names that use this object.
   * - ``OnEquip``
     - ``false``
     - ``list``
     - 
     - List of conditions and effects that will be executed when the object is equiped.
   * - ``OnAddedInventory``
     - ``false``
     - ``list``
     - 
     - List of conditions and effects that will be executed when the object is added to the inventory.
   * - ``OnUseSlot``
     - ``false``
     - ``list``
     - 
     - List of conditions and effects that will be executed when the object is used from the slot.
   * - ``OnUseInventory``
     - ``false``
     - ``list``
     - 
     - List of conditions and effects that will be executed when the object is used from the inventory.
   * - ``OnTickActions``
     - ``false``
     - ``list``
     - 
     - List of actions executed every tick from this entity.