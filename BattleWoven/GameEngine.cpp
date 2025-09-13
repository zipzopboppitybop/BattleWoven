#include "GameEngine.h"

GameEngine::GameEngine(const std::string& path) 
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	mAssets.loadFromFile(path);

	mWindow.create(sf::VideoMode({ mAssets.mWindowConfig.W, mAssets.mWindowConfig.H }), "BattleWoven");
	mWindow.setFramerateLimit(60);
	mView.setSize({ (float)mWindow.getSize().x, (float)mWindow.getSize().y });
	mWindow.setView(mView);

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
	player->add<CAnimation>(mAssets.mAnimationMap.at("StandingDown").get());
	player->add<CTransform>(Vec2f(mWindow.getSize().x / 2, mWindow.getSize().y / 2), Vec2f(5,5), Vec2f(3.0f, 3.0f), 1);
	player->add<CInput>();
	player->add<CAbility>();
	player->add<CHealth>(100);
	player->add<CState>("Standing Down");
	player->get<CAbility>().abilities.push_back(mAssets.mAbilitiesMap["Fireball"]);
}

void GameEngine::run()
{
	while (mRunning && mWindow.isOpen())
	{
		float deltaTime = mDeltaClock.restart().asSeconds();

		mEntities.update();

		ImGui::SFML::Update(mWindow, sf::seconds(deltaTime));

		sUserInput();
		sMovement();
		sAnimation(deltaTime);
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

void GameEngine::sAnimation(float deltaTime) 
{
	for (auto& entity : mEntities.getEntities()) 
	{
		if (!entity->has<CAnimation>()) continue;

		auto& anim = entity->get<CAnimation>();

		if (!anim.prototype) continue;

		anim.state.elapsed += deltaTime;
		if (anim.state.elapsed >= anim.prototype->frameTime()) 
		{
			anim.state.elapsed -= anim.prototype->frameTime();
			anim.state.currentFrame = (anim.state.currentFrame + 1) % anim.prototype->frameCount();
		}

		anim.prototype->setFrame(anim.state.currentFrame);
	}
}

void GameEngine::sRender()
{
	mWindow.clear();

	for (auto& entity : mEntities.getEntities())
	{
		if (!entity->has<CAnimation>() || !entity->has<CTransform>()) continue;

		auto& anim = entity->get<CAnimation>();
		auto& transform = entity->get<CTransform>();

		sf::Sprite sprite = anim.prototype->sprite();
		sprite.setPosition(transform.pos);
		sprite.setScale(transform.scale);

		mWindow.draw(sprite);
	}
}

void GameEngine::sMovement()
{
	if (!player() || !player()->isActive())
	{
		return;
	}

	auto& playerInput = player()->get<CInput>();
	auto& playerMovement = player()->get<CTransform>();
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
		auto& playerState = player()->get<CState>();
		auto& anim = player()->get<CAnimation>(); 

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->scancode)
			{
			case sf::Keyboard::Scancode::D:
				playerInput.right = true;
				if (playerState.state != "Walking Right") {
					playerState.state = "Walking Right";
					anim.prototype = mAssets.mAnimationMap.at("WalkingRight").get();
					anim.state = AnimationState{};
				}
				break;
			case sf::Keyboard::Scancode::A:
				playerInput.left = true;
				if (playerState.state != "Walking Left") {
					playerState.state = "Walking Left";
					anim.prototype = mAssets.mAnimationMap.at("WalkingLeft").get();
					anim.state = AnimationState{};
				}
				break;
			case sf::Keyboard::Scancode::W:
				playerInput.up = true;
				if (playerState.state != "Walking Up") {
					playerState.state = "Walking Up";
					anim.prototype = mAssets.mAnimationMap.at("WalkingUp").get();
					anim.state = AnimationState{};
				}
				break;
			case sf::Keyboard::Scancode::S:
				playerInput.down = true;
				if (playerState.state != "Walking Down") {
					playerState.state = "Walking Down";
					anim.prototype = mAssets.mAnimationMap.at("WalkingDown").get();
					anim.state = AnimationState{};
				}
				break;
			case sf::Keyboard::Scancode::Space:
				setPaused(!mPaused);
				break;
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
			default: break;
			}

			if (playerInput.right) {
				playerState.state = "Walking Right";
				anim.prototype = mAssets.mAnimationMap.at("WalkingRight").get();
			}
			else if (playerInput.left) {
				playerState.state = "Walking Left";
				anim.prototype = mAssets.mAnimationMap.at("WalkingLeft").get();
			}
			else if (playerInput.up) {
				playerState.state = "Walking Up";
				anim.prototype = mAssets.mAnimationMap.at("WalkingUp").get();
			}
			else if (playerInput.down) {
				playerState.state = "Walking Down";
				anim.prototype = mAssets.mAnimationMap.at("WalkingDown").get();
			}
			else {
				if (playerState.state == "Walking Right") {
					playerState.state = "Standing Right";
					anim.prototype = mAssets.mAnimationMap.at("StandingRight").get();
				}
				else if (playerState.state == "Walking Left") {
					playerState.state = "Standing Left";
					anim.prototype = mAssets.mAnimationMap.at("StandingLeft").get();
				}
				else if (playerState.state == "Walking Up") {
					playerState.state = "Standing Up";
					anim.prototype = mAssets.mAnimationMap.at("StandingUp").get();
				}
				else if (playerState.state == "Walking Down") {
					playerState.state = "Standing Down";
					anim.prototype = mAssets.mAnimationMap.at("StandingDown").get();
				}
			}

			anim.state = AnimationState{};
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

std::shared_ptr<Entity> GameEngine::player()
{
	auto& players = mEntities.getEntities("player");
	if (!players.empty())
	{
		return players.front();
	}
	return nullptr;
}

sf::RenderWindow& GameEngine::window()
{
	return mWindow;
}

