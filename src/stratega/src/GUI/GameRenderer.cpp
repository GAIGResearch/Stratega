#include <cassert>

#include <Stratega/GUI/GameRenderer.h>
#ifndef BUILD_HEADLESS
#include <Stratega/GUI/RTSGameRenderer.h>
#include <Stratega/GUI/TBSGameRenderer.h>
#endif
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	std::unique_ptr<GameRenderer> createRenderer(const GameType& type, SGA::Vector2i& resolution)
	{
#ifndef BUILD_HEADLESS
		if (type == GameType::TBS)
		{
			return std::make_unique<TBSGameRenderer>(resolution);
		}
		if (type == GameType::RTS)
		{
			return std::make_unique<RTSGameRenderer>(resolution);
		}
#else
		throw std::runtime_error("Stratega was compiled in headless mode, GUI is not available.");
#endif
		assert(false);
		return nullptr;
	}

}
