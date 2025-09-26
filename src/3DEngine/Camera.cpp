#include "Camera.hpp"
#include "Engine3D.hpp"
#include "engineConfiguration.hpp"
#include "functions.hpp"
#include "../configuration.hpp"

sf::Vector3f Camera::position = engineConf::initialCameraPosition;
vector<float> Camera::rotation = {0, 0, 0};
vector<float> Camera::rotationBuffer = engineConf::initialCameraRotation;
sf::Vector3f Camera::direction = Engine3D::rotateWithEulerAngles({0, 0, -1}, rotation);

float Camera::fov = engineConf::fov;

void Camera::setPosition(sf::Vector3f newPosition) {
    position = newPosition;
}

void Camera::setRotation(vector<float> newRotation) {
    rotation = {0, 0, 0};

    rotationBuffer[0] = newRotation[0];
    rotationBuffer[1] = newRotation[1];
    rotationBuffer[2] = newRotation[2];

    sf::Mouse::setPosition(static_cast<sf::Vector2i>(conf::window_size_f) / 2);
}

void Camera::setDirection(sf::Vector3f newDirection) {
    direction = newDirection;
}

void Camera::setFov(float newFov) {
    fov = newFov;
}

sf::Vector3f Camera::getPosition() {
    return position;
}


vector<float> Camera::getRotation() {
    return rotation;
}


sf::Vector3f Camera::getDirection() {
    return direction;
}

float Camera::getFov() {
    return fov;
}

void Camera::updateCamera(sf::RenderWindow& window) {
    rotation[0] = (rotationBuffer[0] + (conf::window_size_f.x / 2 - sf::Mouse::getPosition(window).x) * engineConf::DPI / 1000);
    rotation[1] = (rotationBuffer[1] + (conf::window_size_f.y / 2 - sf::Mouse::getPosition(window).y) * engineConf::DPI / 1000);

    if (distanceBetween(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)), conf::window_size_f / 2.0f) > 100) {
        rotationBuffer[0] -= (sf::Mouse::getPosition(window).x - conf::window_size_f.x / 2.0f) * engineConf::DPI / 1000;
        rotationBuffer[1] -= (sf::Mouse::getPosition(window).y - conf::window_size_f.y / 2.0f) * engineConf::DPI / 1000;
        sf::Mouse::setPosition(static_cast<sf::Vector2i>(conf::window_size_f) / 2);
    }


    sf::Vector3f movementDirection = sf::Vector3f(direction.x, 0, direction.z).normalized();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        position.x += engineConf::movementSpeed * movementDirection.x;
        position.z += engineConf::movementSpeed * movementDirection.z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        position.x += engineConf::movementSpeed * Engine3D::rotateWithEulerAngles(movementDirection, {90, 0, 0}).x;
        position.z += engineConf::movementSpeed * Engine3D::rotateWithEulerAngles(movementDirection, {90, 0, 0}).z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        position.x -= engineConf::movementSpeed * movementDirection.x;
        position.z -= engineConf::movementSpeed * movementDirection.z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        position.x += engineConf::movementSpeed * Engine3D::rotateWithEulerAngles(movementDirection, {-90, 0, 0}).x;
        position.z += engineConf::movementSpeed * Engine3D::rotateWithEulerAngles(movementDirection, {-90, 0, 0}).z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        position.y += engineConf::movementSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        position.y -= engineConf::movementSpeed;
    }

    direction = Engine3D::rotateWithEulerAngles({0, 0, -1}, rotation);
}
