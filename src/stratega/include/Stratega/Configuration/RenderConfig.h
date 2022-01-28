#pragma once
#include <map>
#include <string>
#include <yaml-cpp/yaml.h>
#include <Stratega/Representation/Vector2.h>
namespace SGA
{
	struct GameConfig;

	struct RenderConfig
	{
        std::string outlineShaderPath;
        std::string fontPath;
        std::string entityCircleColliderPath;

        std::string selectedPath;

		std::map<std::string, std::string> entitySpritePaths;
		std::map<std::string, std::string> tileSpritePaths;

		SGA::Vector2i resolution;
	};
}
