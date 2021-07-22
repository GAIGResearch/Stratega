#pragma once
#include <map>
#include <string>
#include <yaml-cpp/yaml.h>

namespace SGA
{
	struct GameConfig;

	struct RenderConfig
	{
        std::string outlineShaderPath;
		std::map<std::string, std::string> entitySpritePaths;
		std::map<std::string, std::string> tileSpritePaths;
	};
}
