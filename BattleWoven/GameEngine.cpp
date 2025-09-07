#include "GameEngine.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
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
	}

	mWindow.create(sf::VideoMode({ mWindowConfig.W, mWindowConfig.H }), "BattleWoven");
	mWindow.setFramerateLimit(60);

	ImGui::SFML::Init(mWindow);

	ImGui::GetStyle().ScaleAllSizes(2.0f);
	ImGui::GetIO().FontGlobalScale = 2.0f;
}

void GameEngine::run()
{
	sRender();
}

void GameEngine::sRender()
{
	while (mWindow.isOpen()) {

		while (const auto event = mWindow.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				mWindow.close();
			}
		}

		mWindow.clear();

		mWindow.display();
	}
}

sf::Window& GameEngine::window()
{
	return mWindow;
}