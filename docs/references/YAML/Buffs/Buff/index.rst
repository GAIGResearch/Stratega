.. _yaml-buff:

Buff
===========

:Buff: Contais the definition the buff.

:YAML Key: Buff

:Example:
.. code-block:: yaml

    TestBuff:
        MultiplicationModifier:
            Health: 1.5
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
   * - ``List``
     - ``false``
     - ``Map``
     -  MultiplicationModifier/AdditiveModifier
     - List of modifiers.