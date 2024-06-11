#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
//#include "Game.h"
#include "GameEngine.h"

template <typename T>
bool isShapeTouchingEdge(const T shape, const int width, const int height) {
    return shape.getPosition().x <= 0 || shape.getPosition().y <= 0 || (shape.getPosition().x + shape.getLocalBounds().width) >= width || (shape.getPosition().y + shape.getLocalBounds().height) >= height;
}

template <typename T>
bool isShapeTouchingEdgeVertical(const T shape, const int height) {
    return shape.getPosition().y <= 0 || (shape.getPosition().y + shape.getLocalBounds().height) >= height;
}

template <typename T>
bool isShapeTouchingEdgeHorizontal(const T shape, const int width) {
    return shape.getPosition().x <= 0 || (shape.getPosition().x + shape.getLocalBounds().width) >= width;
}

void screenSaver();

int main()
{
    //screenSaver();
    
    //Run game with debug console messages
    //Game game("./config.txt",true);
    //game.run();
    GameEngine gameEngine("");
    gameEngine.run();
    return 0;
}

void screenSaver() {
    const int wWidth = 2560;
    const int wHeight = 1440;
    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Cool Epic Screensaver!", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::CircleShape circle(50);
    circle.setPosition(300.0f, 300.0f);
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(2);
    float circleHorizMoveSpeed = 1.5f;
    float circleVertMoveSpeed = 1.2f;

    const std::vector<sf::Color> colors{ sf::Color::Red, sf::Color(255,165,0), sf::Color::Yellow, sf::Color::Green, sf::Color::Blue, sf::Color(75,0,130), sf::Color(128,0,128) };
    int colorIndex = 0;
    circle.setFillColor(colors[colorIndex]);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                std::cout << "Key pressed with code = " << event.key.code << '\n';

                if (event.key.code == sf::Keyboard::X) {
                    circleHorizMoveSpeed *= -1.0f;
                    circleVertMoveSpeed *= -1.0f;
                }
                if (event.key.code == sf::Keyboard::Up) {
                    std::cout << "Sped up\n";
                    circleHorizMoveSpeed *= 2.0f;
                    circleVertMoveSpeed *= 2.0f;
                }
                if (event.key.code == sf::Keyboard::Down) {
                    std::cout << "Slowed down\n";
                    circleHorizMoveSpeed *= 0.5f;
                    circleVertMoveSpeed *= 0.5f;
                }
            }
        }
        bool touched = false;
        if (isShapeTouchingEdgeVertical(circle, wHeight)) {
            circleVertMoveSpeed *= -1.0f;
            touched = true;
        }
        if (isShapeTouchingEdgeHorizontal(circle, wWidth)) {
            circleHorizMoveSpeed *= -1.0f;
            touched = true;
        }
        if (touched) {
            colorIndex++;
            if (colorIndex >= colors.size()) {
                colorIndex = 0;
            }
            circle.setFillColor(colors[colorIndex]);
        }

        circle.setPosition(circle.getPosition().x + circleHorizMoveSpeed, circle.getPosition().y + circleVertMoveSpeed);
        window.clear();
        window.draw(circle);
        window.display();
    }
}