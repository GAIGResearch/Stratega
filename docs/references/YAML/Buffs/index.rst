.. _yaml-buffs:

Buffs
===========

:Buffs: Contains the definition of the buffs used in the game.
:YAML Key: Tiles

:Example:
.. code-block:: yaml

    Buffs: # Buff lists
        HealthBuff:
            MultiplicationModifier:
                Health: 1.5
        HealthBoostBuff:
            MultiplicationModifier:
                Health: 1.1
        AttackBoostBuff:
            AdditiveModifier: 
                AttackDamage: 20
                AttackRange: 2


:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - :ref:`Buff <yaml-buff>`
     - ``false``
     - ``map``
     - 
     - Buff definition
