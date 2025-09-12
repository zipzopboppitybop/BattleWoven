#pragma once

#include "Entity.hpp"
#include "Vec2.hpp"
#include "Entity_Manager.hpp"
#include "Assets.hpp"
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

class GameEngine
{
protected:
	sf::RenderWindow mWindow;
	sf::Clock mDeltaClock;
	sf::View mView;
	EntityManager mEntities;
	Assets mAssets;
	bool mPaused = false;
	bool mRunning = true;
	bool mShowImgui = true;
	int mCurrentFrame = 0;;


	void init(const std::string& path);
	void sRender();
	void spawnPlayer();
	void sUserInput();
	void setPaused(bool paused);
	void sMovement();
	void sGui();

	std::shared_ptr<Entity> player();
public:

	sf::Sprite mSprite;
	sf::Texture texture;
	GameEngine(const std::string& path);

	void run();
	sf::RenderWindow& window();
};