#include <SFML/Graphics.hpp>
#include <bitset>
#include <iostream>

#include "configuration.hpp"
#include "events.hpp"
#include "3DEngine/Camera.hpp"
#include "3DEngine/Engine3D.hpp"

using namespace std;

int main() {
    auto window = sf::RenderWindow(sf::VideoMode(conf::window_size), "3DEngine", sf::State::Fullscreen);
    window.setFramerateLimit(conf::maxFps);
    window.setMouseCursorVisible(false);
    sf::Mouse::setPosition(static_cast<sf::Vector2i>(conf::window_size) / 2);

    Engine3D::generateBox("box1", {0, 0, 0}, {3, -1, 3});
    Engine3D::generateBox("box2", {400, 0, 100}, {1, 3, -3});

    while (window.isOpen())
    {
        manageEvents(window);


        Camera::updateCamera(window);
        Engine3D::render();


        window.clear(sf::Color(100, 100, 100));

        Engine3D::draw(window);

        window.display();

    }
}
