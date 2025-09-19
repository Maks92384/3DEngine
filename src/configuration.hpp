#pragma once

namespace conf {
    // window configuration

    sf::Vector2u const window_size = {1920, 1080};
    sf::Vector2f const window_size_f = static_cast<sf::Vector2f>(window_size);
    unsigned int const maxFps = 144;
}