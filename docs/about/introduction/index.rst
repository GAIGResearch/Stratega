#############
Introduction
#############

Stratega aims to provide a fast and flexible framework for researching AI in complex strategy games.
Games are configured using YAML-files and can be played through a GUI or by agents using an API.
Stratega allows creating a wide variety of turn-based and real-time strategy games.
Due to the flexibility of using YAML-files, the user can design and run various games for testing agents without adjusting it to the game.

The framework has been built with a focus of statistical forward planning (SFP) agents.
For this purpose, agents can access and modify game states and use the forward model to simulate the game.
Thanks to the ability to configure a wide range of games and access to the forward model, Stratega is perfectly suited for researching general game playing in complex games.

++++++
Games
++++++
Visit the :ref:`games-section <games>` to see games that have been configured using Stratega.
If you want to know more about how to configure your own games, visit the :ref:`YAML-reference <yaml-reference>`.