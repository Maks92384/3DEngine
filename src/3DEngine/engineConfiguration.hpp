#pragma once
#include "Camera.hpp"

namespace engineConf {
    // Overall engine configuration

    static const bool debugMode = true;


    // Camera configuration

    static const sf::Vector3f initialCameraPosition = {0, -300, 1000};
    static const vector<float> initialCameraRotation = {0, 0, 0};

    static const float fov = 90; // fov in degrees

    static const float movementSpeed = 10.0f;


    // Object configuration

    static const float objectScale = 100.0f;
}
