#include <iostream>
#include <fstream>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "Entity_Manager.hpp"


int main() 
{
    EntityManager Entities;
    auto player = Entities.addEntity("player");
    player->add<CTransform>(Vec2f(500.0f, 500.0f));
    player->add<CShape>(50.0f, 8, sf::Color::Blue, sf::Color::Blue, 1);
    
    // Create a window with the given width and height
    const int wWidth = 1280;
    const int wHeight = 720;
    sf::RenderWindow window(sf::VideoMode({ wWidth, wHeight }), "Shapes");

    // Cap Frame Rate 
    window.setFramerateLimit(60);

    // Initialize ImGui 
    ImGui::SFML::Init(window);

    // Scale ImGui ui and text size by 2
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    sf::Font font;
    if (!font.openFromFile("fonts/Roboto-Regular.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    // Clock for internal timing
    sf::Clock deltaClock;

    //Main Loop
    while (window.isOpen()) {
        // Event handler
        while (const auto event = window.pollEvent()) {
            // pass event for ImGui to parse
            ImGui::SFML::ProcessEvent(window, *event);
            // this event happens when window is closes
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Update ImGui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());

        // ImGui Ui
        ImGui::Begin("Window Title");
        ImGui::Text("Window Text!");
        ImGui::SameLine();
        // End of Ui box
        ImGui::End();

        // clear window every frame to get rid of drawn shapes
        window.clear();
        auto& shape = player->get<CShape>().circle;
        auto& transform = player->get<CTransform>().pos;
        shape.setPosition(transform);
        window.draw(shape);

        // Draw ui last so it's on top
        ImGui::SFML::Render(window);

        // display window
        window.display();
    }

    // Make sure imgui gets destroyed when sfml quits out
    ImGui::SFML::Shutdown();

    return 0;
}