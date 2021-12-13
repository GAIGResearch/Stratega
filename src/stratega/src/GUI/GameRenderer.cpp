#include <cassert>

#include <Stratega/GUI/GameRenderer.h>
#ifdef BUILD_GUI
#include <Stratega/GUI/RTSGameRenderer.h>
#include <Stratega/GUI/TBSGameRenderer.h>
#endif
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	std::unique_ptr<GameRenderer> createRenderer(const GameType& type, SGA::Vector2i& resolution)
	{
#ifdef BUILD_GUI
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
