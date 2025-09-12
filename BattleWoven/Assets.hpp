#pragma once

#include <map>
#include <string>
#include <iostream> 
#include <fstream>

#include <SFML/Graphics.hpp>

struct WindowConfig { unsigned int W, H; };

class Assets
{

public:
	WindowConfig mWindowConfig;
	std::map<std::string, sf::Texture> mTextureMap;


	Assets() = default;

	void addTexture(const std::string& textureName, const std::string& texturePath)
	{
		sf::Texture texture(texturePath);

		if (!texture.loadFromFile(texturePath))
		{
			std::cerr << "Cannot load texture!";
		}

		mTextureMap[textureName] = texture;
	}

	void loadFromFile(const std::string& path)
	{
		std::ifstream config(path);
		std::string temp;

		if (!config.is_open()) {
			std::cerr << "Error opening file!" << std::endl;
		}

		while (config >> temp)
		{
			if (temp == "Window")
			{
				config >> mWindowConfig.W >> mWindowConfig.H;
			}

			if (temp == "Texture")
			{
				std::string textureName, texturePath;

				config >> textureName >> texturePath;

				addTexture(textureName, texturePath);
			}
		}

	}
};