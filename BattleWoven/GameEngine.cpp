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

void GameEngine::setPaused(bool paused)
{
	mPaused = paused;
}

void GameEngine::spawnPlayer()
{
	auto player = mEntities.addEntity("player");
	player->add<CTransform>(Vec2f(mWindow.getSize().x / 2, mWindow.getSize().y / 2));
	player->add<CShape>(100.f, 8, sf::Color::Blue, sf::Color::Blue, 5);
	player->add<CInput>();
}

void GameEngine::run()
{
	while (mRunning && mWindow.isOpen())
	{
		mEntities.update();

		sRender();
		sUserInput();
		sMovement();

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

void GameEngine::sMovement()
{
	if (!player())
	{
		return;
	}

	auto& playerInput = player()->get<CInput>();
	auto& playerMovement = player()->get<CTransform>();
	auto& playerCollisionRadius = player()->get<CCollision>().radius;
	auto windowSize = mWindow.getSize();

	playerMovement.velocity.x = 0;
	playerMovement.velocity.y = 0;

	if (playerInput.right)
	{
		playerMovement.velocity.x = 5;
	}

	if (playerInput.left)
	{
		playerMovement.velocity.x = -5;
	}

	if (playerInput.up)
	{
		playerMovement.velocity.y = -5;
	}

	if (playerInput.down)
	{
		playerMovement.velocity.y = 5;
	}


	for (auto& entity : mEntities.getEntities())
	{
		if (entity->isActive())
		{
			auto& transform = entity->get<CTransform>();
			transform.pos.x += transform.velocity.x;
			transform.pos.y += transform.velocity.y;
		}
	}
}

void GameEngine::sUserInput()
{
	while (const std::optional event = mWindow.pollEvent())
	{
		ImGui::SFML::ProcessEvent(mWindow, *event);

		if (event->is<sf::Event::Closed>())
		{
			mWindow.close();
		}

		if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
		{
			continue;
		}

		if (!player())
		{
			return;
		}

		auto& playerInput = player()->get<CInput>();

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->scancode)
			{
			case sf::Keyboard::Scancode::D:
				playerInput.right = true;
				break;
			case sf::Keyboard::Scancode::A:
				playerInput.left = true;
				break;
			case sf::Keyboard::Scancode::W:
				playerInput.up = true;
				break;
			case sf::Keyboard::Scancode::S:
				playerInput.down = true;
				break;
			case sf::Keyboard::Scancode::Space:
				setPaused(!mPaused);
				break;
			//case sf::Keyboard::Scancode::Grave:
			//	mShow_imgui = !mShow_imgui;
			//	break;
			default:
				break;
			}
		}
		else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			switch (keyReleased->scancode)
			{
			case sf::Keyboard::Scancode::D:
				playerInput.right = false;
				break;
			case sf::Keyboard::Scancode::A:
				playerInput.left = false;
				break;
			case sf::Keyboard::Scancode::W:
				playerInput.up = false;
				break;
			case sf::Keyboard::Scancode::S:
				playerInput.down = false;
				break;
			default:
				break;
			}
		}
	}
}
sf::RenderWindow& GameEngine::window()
{
	return mWindow;
}

std::shared_ptr<Entity> GameEngine::player()
{
	auto& players = mEntities.getEntities("player");
	if (!players.empty())
	{
		return players.front();
	}
	return nullptr;
}