.. _yaml-entities:

Objects
===========

:Entities: Contains the definition of the objects used in the game.
:YAML Key: Objects

:Example:
.. code-block:: yaml

    Objects:
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
   * - :ref:`Entity <yaml-entity>`
     - ``false``
     - ``map``
     - 
     - Entity type definition
