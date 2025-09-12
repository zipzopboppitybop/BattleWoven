#pragma once

#include <map>
#include <string>
#include <iostream> 
#include <fstream>

#include <SFML/Graphics.hpp>
#include "Components.hpp"
#include "Animation.hpp"

struct Ability;

struct WindowConfig { unsigned int W, H; };

class Assets
{

public:
	WindowConfig mWindowConfig = {0,0};
	std::map<std::string, sf::Texture> mTextureMap;
	std::map<std::string, Ability> mAbilitiesMap;
	std::map<std::string, std::unique_ptr<Animation>> mAnimationMap;

	Assets() = default;

	void addTexture(const std::string& textureName, const std::string& texturePath)
	{
		sf::Texture texture;
		if (!texture.loadFromFile(texturePath))
		{
			std::cerr << "Cannot load texture: " << texturePath << std::endl;
		}
		mTextureMap.emplace(textureName, std::move(texture));
	}

	void addAbility(const std::string& abilityName, int damage, int level, float cooldown)
	{
		Ability ability{ abilityName, true, damage, level, cooldown };

		mAbilitiesMap.emplace(abilityName, std::move(ability));
	}

	void addAnimation(const std::string& animationName, const std::string& textureName, int x, int y, int width, int height, int frameCount, float frameTime)
	{
		sf::Texture& texture = mTextureMap[textureName];

		auto anim = std::make_unique<Animation>(animationName, texture, Vec2f(x, y), Vec2f(width, height), frameCount, frameTime);

		mAnimationMap.emplace(animationName,std::move(anim));
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

			if (temp == "Ability")
			{
				std::string abilityName;
				int damage, level;
				float cooldown = 0.0f;

				config >> abilityName >> damage >> level >> cooldown;

				addAbility(abilityName, damage, level, cooldown);
			}

			if (temp == "Animation")
			{
				std::string animationName, textureName;
				int x, y, width, height, frameCount;
				float frameTime;

				config >> animationName >> textureName >> x >> y >> width >> height >> frameCount >> frameTime;

				addAnimation(animationName, textureName, x, y, width, height, frameCount, frameTime);
			}
		}

	}
};