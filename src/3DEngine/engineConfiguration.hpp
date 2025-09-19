#pragma once

namespace engineConf {
    // Overall engine configuration

    static const bool debugMode = false;


    // Camera configuration

    static const sf::Vector3f initialCameraPosition = {200, 150, 300};
    static const vector<float> initialCameraRotation = {35, 0, 0};
    static const float fov = 120; // fov in degrees
    static const float movementSpeed = 10.0f;
    static const float DPI = 100;


    // Object configuration

    static const float objectScale = 100.0f;
}
