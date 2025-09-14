#include <SFML/Graphics.hpp>

#include "events.hpp"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "3DEngine");
    window.setFramerateLimit(144);

    while (window.isOpen())
    {
        manageEvents(window);

        window.clear();
        window.display();
    }
}
