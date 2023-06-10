# Stratega
[![Documentation Status](https://readthedocs.org/projects/stratega/badge/?version=latest)](https://stratega.readthedocs.io/en/latest/?badge=latest)
[![pypi releases](https://img.shields.io/pypi/v/stratega.svg)](https://pypi.org/project/stratega)
[![Builds](https://github.com/GAIGResearch/Stratega/actions/workflows/ci.yml/badge.svg?branch=dev)](https://github.com/GAIGResearch/Stratega/actions/workflows/ci.yml)
[![Python Wheel Builds](https://github.com/GAIGResearch/Stratega/actions/workflows/wheels.yml/badge.svg?branch=dev)](https://github.com/GAIGResearch/Stratega/actions/workflows/wheels.yml)

Stratega aims to provide a fast and flexible framework for researching AI in complex strategy games. Games are configured using YAML-files and can be played through a GUI or by agents using an API. Stratega allows creating a wide variety of turn-based and real-time strategy games. Due to the flexibility of using YAML-files, the user can design and run various games for testing agents without adjusting it to the game.

The framework has been built with a focus of statistical forward planning (SFP) agents. For this purpose, agents can access and modify game states and use the forward model to simulate the game. Thanks to the ability to configure a wide range of games and access to the forward model, Stratega is perfectly suited for researching general game playing in complex games.

# Community

Join the Discord community for help and to talk about what you are doing with Stratega!

[![Discord Chat](https://img.shields.io/discord/783231009738719233.svg)](https://discord.gg/Y2uZZ3TSuT)

## Documentation

Full documentation can be found here:

[stratega.readthedocs.io](https://stratega.readthedocs.io/)

# Gallery

PushThemAll(TBS)
![TBS](/images/tbsPushThemAll.gif)

KillTheKing(TBS)
![TBS](/images/tbsKillTheKing.gif)

BasicRTS(RTS)
![RTS](/images/rtsBasic.gif)

# Experiments of IEEE TOG paper: Elastic Monte Carlo Tree Search
Please check the `tog_ktkpta` branch for *KillTheKing* and *Pusher* games. *Two Kingdoms* is in the `tog_tk` branch.

# Research based on Stratega
```
@ARTICLE{10143265,
  author={Xu, Linjie and Dockhorn, Alexander and Perez-Liebana, Diego},
  journal={IEEE Transactions on Games}, 
  title={Elastic Monte Carlo Tree Search}, 
  year={2023},
  volume={},
  number={},
  pages={1-11},
  doi={10.1109/TG.2023.3282351}}
 
@INPROCEEDINGS{9893587,
  author={Xu, Linjie and Hurtado-Grueso, Jorge and Jeurissen, Dominic and Liebana, Diego Perez and Dockhorn, Alexander},
  booktitle={2022 IEEE Conference on Games (CoG)}, 
  title={Elastic Monte Carlo Tree Search with State Abstraction for Strategy Game Playing}, 
  year={2022},
  volume={},
  number={},
  pages={369-376},
  doi={10.1109/CoG51982.2022.9893587}}
  
@INPROCEEDINGS{9504824,
  author={Dockhorn, Alexander and Hurtado-Grueso, Jorge and Jeurissen, Dominik and Xu, Linjie and Perez-Liebana, Diego},
  booktitle={2021 IEEE Congress on Evolutionary Computation (CEC)}, 
  title={Portfolio Search and Optimization for General Strategy Game-Playing}, 
  year={2021},
  volume={},
  number={},
  pages={2085-2092},
  doi={10.1109/CEC45853.2021.9504824}}
```

