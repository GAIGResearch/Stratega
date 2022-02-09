Frequently asked questions
##########################

Disable fog of war
==================
        
By default Stratega pass to agents the gamestate with fog of war applied, this option can be disabled in the yaml configuration:

.. code-block:: yaml
    :caption: Note that this configuration is incomplete.

    GameConfig:
        Type: TBS
        RoundLimit: 100
        FogOfWar: false

        ....

.. list-table:: FogOfWar
   :header-rows: 1

   * - Enabled
     - Disabled
   * - .. image:: ../../images/fog.png
            :width: 400
     - .. image:: ../../images/withoutFog.png
            :width: 400

How to cite this project?
=========================

We suggest the following BibTeX template to cite Stratega in scientific
discourse:

.. code-block:: bash

    @misc{Stratega,
       author = {Diego Perez-Liebana, Alexander Dockhorn, Jorge Hurtado Grueso, Dominik Jeurissen},
       year = {2021},
       note = {https://github.com/GAIGResearch/Stratega},
       title = {Stratega -- fast and flexible framework for researching AI in complex strategy games}
    }
    {"mode":"full","isActive":false}