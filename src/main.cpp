#include <SFML/Graphics.hpp>

#include "configuration.hpp"
#include "events.hpp"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode(conf::window_size), "3DEngine");
    window.setFramerateLimit(conf::maxFps);

    while (window.isOpen())
    {
        manageEvents(window);

        window.clear();
        window.display();
    }
}
