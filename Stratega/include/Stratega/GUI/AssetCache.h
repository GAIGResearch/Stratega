#pragma once
#include <map>
#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>


class AssetCache
{

public:
	//assets/textures/
	void loadTexture(std::string name, std::string fileName);
	void loadFont(std::string name, std::string fileName);
	
	sf::Texture& getTexture(std::string name);
	sf::Font& getFont(std::string name);

private:
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::Font> fonts;

};
