#pragma once

#include "Entity.hpp"
#include "Vec2.hpp"
#include "Entity_Manager.hpp"
#include <string>
#include <iostream> 
#include <fstream>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

struct WindowConfig { unsigned int W, H; };

class GameEngine
{
protected:
	sf::RenderWindow mWindow;
	sf::Clock mDeltaClock;
	sf::View mView;
	EntityManager mEntities;
	bool mPaused = false;
	bool mRunning = true;
	WindowConfig mWindowConfig;

	void init(const std::string& path);
	void sRender();
	void spawnPlayer();
	void sUserInput();
	void setPaused(bool paused);
	void sMovement();

	std::shared_ptr<Entity> player();
public:
	GameEngine(const std::string& path);

	void run();
	sf::RenderWindow& window();
};