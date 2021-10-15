####################
Installation
####################

.. note::
    We currently do not offer any binaries. Instead, the source code has to be compiled manually. We are working on simplifying the installation of the framework.

+++++++
Windows 
+++++++

Using command-line:
*******************

#. First lets clone the repository and change the directory to Stratega.

    .. code-block:: bash

        git clone https://github.com/GAIGResearch/Stratega.git
        cd Stratega

#. If you want to check the last updated (not stable) branch checkout dev.

    .. code-block:: bash

        git checkout dev  
        
#. Lets configure cmake by running the following code.

    .. code-block:: bash

        cmake . -B out

#. Finally change the directory to the output and build the project with

    .. code-block:: bash

        cd out
        cmake --build .


Using Visual Studio 2019:
************************

Stratega is compiled using CMake. Luckily Visual Studio already provides a simple way to do this. 

#. Install Visual Studio 2019 Community from `here <https://visualstudio.microsoft.com/downloads/>`_
#. Clone Stratega from `here <https://github.com/GAIGResearch/Stratega/>`_ (use the master/dev branch)
#. In Visual Studio open the folder (not project) containing the Stratega source code
#. Execute **Project->Generate Cache**, Visual Studio may automatically do this when opening the folder
#. Execute **Build->Build All** to generate the executables
#. Under **Select Startup Item...** choose GUI.exe and press **F5** to run it

++++++++++++++++++++++++++++++++
Mac OS
++++++++++++++++++++++++++++++++

#. First lets clone the repository and change the directory to Stratega.

    .. code-block:: bash

        git clone https://github.com/GAIGResearch/Stratega.git
        cd Stratega

#. If you want to check the last updated (not stable) branch checkout dev.

    .. code-block:: bash

        git checkout dev  

#. Lets configure cmake by running the following code.

    .. code-block:: bash

        cmake . -B out

#. Finally change the directory to the output and build the project with

    .. code-block:: bash

        cd out
        make

+++++
Linux
+++++

.. note::
    Coming Soon

+++++++++++++++++++++++++++++++++++++++
Compiling with documentation (Advanced)
+++++++++++++++++++++++++++++++++++++++
Stratega generates documentation using Doxygen (and some other related dependencies). By default, the generation of documentation is **disabled** (see  `CMakeLists.txt <https://github.com/GAIGResearch/Stratega/blob/master/CMakeLists.txt>`_, STRATEGA_BUILD_DOCS set to OFF). 

If you wish to generate documentation, you must:

#. Install `Doxygen <https://www.doxygen.nl/download.html>`_ manually. 
#. If, despite installing Doxygen, the **Generate Cache** process still gives any error, follow these complete `installation instructions <https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/>`_ for potentially missing dependencies.
#. Finally, set ENABLE_BUILD_DOCS to ON compile the framework starting from the **Project->Generate Cache** step.
