#pragma once

#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/NewGUI/SGARenderTarget.h>
#include <Stratega/NewGUI/ResourceManager.h>
#include <Stratega/NewGUI/World.h>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

namespace SGA
{
	struct SGAWidget
	{
		SGAWidget(const std::string& newName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm) :
			name(newName), window(newWindow), world(newWorld), fm(newFm)
		{
		}
		virtual ~SGAWidget() = default;

		virtual void update(const GameState& state) = 0;
		virtual void render(SGARenderTarget& renderTarget) = 0;

		// Event Handling
		virtual void mouseScrolled(const sf::Event& /*event*/)
		{

		}
		virtual void mouseButtonReleased(const sf::Event& /*event*/)
		{

		}
		virtual void mouseButtonPressed(const sf::Event& /*event*/)
		{

		}
		virtual void mouseMoved(const sf::Event& /*event*/)
		{

		}

		bool enabled = true;
		const std::string name;
		sf::RenderWindow& window;
		World& world;
		const ForwardModel* fm;
	};


	struct GridInformationWidget : public SGAWidget
	{
		GridInformationWidget(const std::string widgetName, sf::RenderWindow& newWindow, World& newWorld, ForwardModel* newFm, std::unordered_set<int>& newSelectedEntities);

		void update(const GameState& state) override;
		void render(SGARenderTarget& renderTarget) override;

	private:
		bool drawEntityInformation = true;
		bool drawTileInformation = true;
		const GameState* currentGameState;
		std::unordered_set<int>& selectedEntities;
	};
}