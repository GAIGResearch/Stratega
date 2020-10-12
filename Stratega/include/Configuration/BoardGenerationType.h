#pragma once
#include <yaml-cpp/node/node.h>

namespace SGA
{
	enum class BoardGenerationType
	{
		None,
		Manual
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::BoardGenerationType>
    {
        static bool decode(const Node& node, SGA::BoardGenerationType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
        	if(type == "Manual")
        	{
                rhs = SGA::BoardGenerationType::Manual;
        	}
            else
            {
                return false;
            }
        	
            return true;
        }
    };
}