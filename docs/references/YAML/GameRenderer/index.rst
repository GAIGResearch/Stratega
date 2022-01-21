.. _yaml-gamerenderer:

GameRenderer
===========

:GameRenderer: Contains the options of the GUI.
:YAML Key: GameRenderer

:Example:
.. code-block:: yaml

    GameRenderer:
      Resolution:
          Width: 800
          Height: 600
      Default Assets:
          Selected: ../../assets/Tiles/selected.png
          FogOfWar: ../../assets/Tiles/notVisible.png
      #Optional:
      #Font: ../../assets/arial.ttf
      #OutlineShader: ../../assets/OutLine.frag

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Resolution``
     - ``true``
     - ``object``
     - 
     -  Defines the window resolution
   * - ``Default Assets``
     - ``true``
     - ``object``
     - 
     - Defines the default assets
   * - ``Font``
     - ``false``
     - ``string``
     - 
     - Path to the font used by the GUI
   * - ``OutlineShader``
     - ``false``
     - ``string``
     -
     - Path to the outlineshader used to outline the entities


Resolution
*********

:Resolution: Contains the width and height of the GUI.
:YAML Key: Resolution

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Width``
     - ``true``
     - ``int``
     - 
     - Window width
   * - ``Height``
     - ``true``
     - ``int``
     - 
     - Window height

Default Assets
**************

:Default Assets: Contains the definitions of the two sprites (Selected and FogOfWar) used by GUI.
:YAML Key: Default assets

:Properties:

.. list-table::

   * - **Property**
     - **Required**
     - **Data type**
     - **Options**
     - **Definition**
   * - ``Selected``
     - ``true``
     - ``string``
     - 
     - Path to the selected sprite used to outline the entities
   * - ``FogOfWar``
     - ``true``
     - ``string``
     - 
     - Path to the fog of war sprite used to outline the entities

