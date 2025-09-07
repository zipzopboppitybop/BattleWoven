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

	spawnPlayer();
}

void GameEngine::spawnPlayer()
{
	auto player = mEntities.addEntity("player");
	player->add<CTransform>(Vec2f(mWindow.getSize().x / 2, mWindow.getSize().y / 2));
	player->add<CShape>(100.f, 8, sf::Color::Blue, sf::Color::Blue, 5);
	std::cout << mEntities.getEntities().size();
}

void GameEngine::run()
{
	while (mRunning && mWindow.isOpen())
	{
		mEntities.update();

		while (const auto event = mWindow.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				mRunning = false;
				mWindow.close();
			}
		}
		sRender();

		mWindow.display();
	}
}

void GameEngine::sRender()
{
	mWindow.clear();

	for (auto& entity : mEntities.getEntities())
	{
		auto& shape = entity->get<CShape>().circle;
		auto& transform = entity->get<CTransform>().pos;
		shape.setPosition(transform);

		mWindow.draw(shape);
	}
}

sf::RenderWindow& GameEngine::window()
{
	return mWindow;
}