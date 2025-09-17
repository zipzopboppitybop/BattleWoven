#pragma once

#include <map>
#include <string>
#include <iostream> 
#include <fstream>
#include "json.hpp"

#include <SFML/Graphics.hpp>
#include "Components.hpp"
#include "Animation.hpp"

using json = nlohmann::json;

struct Ability;

struct WindowConfig { unsigned int W, H; };

class Assets
{
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

		mAnimationMap.emplace(animationName, std::move(anim));
	}

public:
	WindowConfig mWindowConfig = {0,0};
	std::map<std::string, sf::Texture> mTextureMap;
	std::map<std::string, Ability> mAbilitiesMap;
	std::map<std::string, std::unique_ptr<Animation>> mAnimationMap;

	Assets() = default;

	void loadFromFile(const std::string& path)
	{
		std::ifstream file(path);
		json config;
		file >> config;

		if (!file.is_open()) {
			throw std::runtime_error("Failed to load level: " + path);
		}

		// Window
		mWindowConfig.W = config["window"]["width"];
		mWindowConfig.H = config["window"]["height"];

		// Load textures
		for (auto& [textureName, texturePath] : config["textures"].items()) {
			std::string path = texturePath.get<std::string>();

			addTexture(textureName, path);
		}

		// Load animations
		for (auto& [animationName, animation] : config["animations"].items()) {
			std::string textureName = animation["texture"];

			// pos = [x, y]
			int x = animation["pos"][0].get<int>();
			int y = animation["pos"][1].get<int>();

			// size = [w, h]
			int width = animation["size"][0].get<int>();
			int height = animation["size"][1].get<int>();

			int frameCount = animation["frames"].get<int>();
			float frameTime = animation["time"].get<float>();

			addAnimation(animationName, textureName, x, y, width, height, frameCount, frameTime);
		}

		// Load abilities
		for (auto& [abilityName, ability] : config["abilities"].items()) {

			int damage = ability["damage"].get<int>();
			int level = ability["level"].get<int>();
			int cooldown = ability["cooldown"].get<int>();

			addAbility(abilityName, damage, cooldown, level);
		}
	}
};