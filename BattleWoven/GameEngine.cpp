#include "GameEngine.h"

GameEngine::GameEngine(const std::string& path) : mSprite(texture)
{
	if (!texture.loadFromFile("../BattleWoven/Assets/Textures/deut2b8-433cbf4e-8bab-40c7-8b85-fdd9ab846bc2.png")) {
		std::cerr << "Failed to load texture!" << std::endl;
	}
	mSprite.setTexture(texture);

	init(path);
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
	mView.setSize({ (float)mWindow.getSize().x, (float)mWindow.getSize().y });
	mWindow.setView(mView);

	ImGui::SFML::Init(mWindow);

	ImGui::GetStyle().ScaleAllSizes(2.0f);
	ImGui::GetIO().FontGlobalScale = 2.0f;


	auto enemy = mEntities.addEntity("enemy");
	enemy->add<CTransform>(Vec2f(100.f, 100.f));
	enemy->add<CShape>(100.f, 8, sf::Color::Red, sf::Color::Red, 5);

	spawnPlayer();

	mSprite.setTextureRect(sf::IntRect({ 300, 0 }, { 64, 128 }));
	mSprite.setPosition({ 100.f, 50.f });
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
	player->add<CAbility>();
	player->add<CHealth>(100);

	Ability fireball{ "Fireball", true, 25, 1, 2.0f };

	player->get<CAbility>().abilities.push_back(fireball);
}

void GameEngine::run()
{
	while (mRunning && mWindow.isOpen())
	{
		mEntities.update();

		ImGui::SFML::Update(mWindow, mDeltaClock.restart());

		sUserInput();
		sMovement();
		sRender();
		sGui();

		mCurrentFrame++;

		ImGui::SFML::Render(mWindow);
		mWindow.display();
		
		if (!player())
		{
			spawnPlayer();
		}
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

	mWindow.draw(mSprite);
}

void GameEngine::sMovement()
{
	if (!player() || !player()->isActive())
	{
		return;
	}

	auto& playerInput = player()->get<CInput>();
	auto& playerMovement = player()->get<CTransform>();
	//auto& playerCollisionRadius = player()->get<CCollision>().radius;
	auto windowSize = mWindow.getSize();


	if (ImGui::GetIO().WantCaptureMouse)
	{
		playerInput.right = false;
		playerInput.left = false;
		playerInput.up = false;
		playerInput.down = false;

		return;
	}

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
			transform.pos += transform.velocity;
		}
	}

	mView.setCenter({ playerMovement.pos.x, playerMovement.pos.y });
	mWindow.setView(mView);
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

void GameEngine::sGui()
{
	if (mShowImgui)
	{
		ImGui::Begin("BattleWoven");

		if (ImGui::BeginTabBar("MyTabBar"))
		{
			//if (ImGui::BeginTabItem("Systems"))
			//{
			//	ImGui::Checkbox("Movement", &mMovement);
			//	ImGui::Checkbox("Lifespan", &mLifespan);
			//	ImGui::Checkbox("Collision", &mCollision);
			//	ImGui::Checkbox("Spawning", &mSpawning);
			//	ImGui::SliderInt("Spawn", &mEnemyConfig.SI, 2, 200);
			//	if (ImGui::Button("Manual Spawn"))
			//	{
			//		spawnEnemy();
			//	}
			//	ImGui::Checkbox("GUI", &mShow_imgui);
			//	ImGui::Checkbox("Rendering", &mRendering);

			//	ImGui::EndTabItem();
			//}
			if (ImGui::BeginTabItem("Entities"))
			{
				if (ImGui::CollapsingHeader("Entities"))
				{
					auto enemies = mEntities.getEntities("enemy");
					ImGui::Indent();
					if (ImGui::TreeNode("Enemies"))
					{
						for (auto& enemy : enemies)
						{
							auto enemyColor = enemy->get<CShape>().circle.getFillColor();
							auto enemyPos = enemy->get<CTransform>().pos;
							int id = enemy->id();
							const std::string& tag = enemy->tag();
							float x = enemyPos.x, y = enemyPos.y;
							ImGui::PushID(id);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(enemyColor.r / 255.0f, enemyColor.g / 255.0f, enemyColor.b / 255.0f, 1.0f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(enemyColor.r / 255.0f, enemyColor.g / 255.0f, enemyColor.b / 255.0f, 0.8f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(enemyColor.r / 255.0f, enemyColor.g / 255.0f, enemyColor.b / 255.0f, 0.6f));
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

							if (ImGui::Button("D", ImVec2(36, 36)))
							{
								enemy->destroy();
							}

							ImGui::PopStyleColor(4);
							ImGui::SameLine();
							ImGui::Text("%d %s (%.0f, %.0f)", id, tag.c_str(), x, y);
							ImGui::PopID();
						}

						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Player"))
					{
						if (player())
						{
							auto playerColor = player()->get<CShape>().circle.getOutlineColor();
							auto playerPos = player()->get<CTransform>().pos;
							int id = player()->id();
							const std::string& tag = player()->tag();
							float x = playerPos.x, y = playerPos.y;

							ImGui::PushID(id);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(playerColor.r / 255.0f, playerColor.g / 255.0f, playerColor.b / 255.0f, 1.0f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(playerColor.r / 255.0f, playerColor.g / 255.0f, playerColor.b / 255.0f, 0.8f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(playerColor.r / 255.0f, playerColor.g / 255.0f, playerColor.b / 255.0f, 0.6f));
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

							if (ImGui::Button("D", ImVec2(36, 36)))
							{
								player()->destroy();
							}

							ImGui::PopStyleColor(4);
							ImGui::SameLine();
							ImGui::Text("%d %s (%.0f, %.0f)", id, tag.c_str(), x, y);
							ImGui::PopID();
						}

						ImGui::TreePop();
					}

					ImGui::Unindent();
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}


sf::RenderWindow& GameEngine::window()
{
	return mWindow;
}

