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

		bool isIsometricGrid;

		Vector2f tileSpriteOrigin;
		Vector2f entitySpriteOrigin;

		Vector2f tileSpriteSize;
		Vector2f entitySpriteSize;

		std::map<std::string, std::string> entitySpritePaths;
		std::map<std::string, std::string> tileSpritePaths;

		SGA::Vector2i resolution;
	};
}