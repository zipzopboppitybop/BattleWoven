#pragma once

#include "Entity.hpp";
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
	WindowConfig mWindowConfig;

	void init(const std::string& path);
	void sRender();

public:
	GameEngine(const std::string& path);

	void run();
	sf::Window& window();
};