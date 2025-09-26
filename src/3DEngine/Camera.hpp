#pragma once
#include <SFML/Graphics.hpp>

using namespace std;

class Camera {
    static sf::Vector3f position;
    static vector<float> rotation; // Yaw, Pitch, Roll in degrees
    static vector<float> rotationBuffer;
    static sf::Vector3f direction; // Normalized vector in XYZ space (right-handed system)

    static float fov;

public:
    static void setPosition(sf::Vector3f newPosition);
    static void setRotation(vector<float> newRotation);
    static void setDirection(sf::Vector3f newDirection);
    static void setFov(float newFov);

    static sf::Vector3f getPosition();
    static vector<float> getRotation();
    static sf::Vector3f getDirection();
    static float getFov();

    static void updateCamera(sf::RenderWindow& window);
};
