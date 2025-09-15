#include "Camera.hpp"

#include <cmath>

#include "Engine3D.hpp"
#include "engineConfiguration.hpp"

sf::Vector3f Camera::position = engineConf::initialCameraPosition;
vector<float> Camera::rotation = engineConf::initialCameraRotation;
sf::Vector3f Camera::direction = Engine3D::rotate({0, 0, -1}, rotation);

float Camera::fov = engineConf::fov;

void Camera::setPosition(sf::Vector3f newPosition) {
    position = newPosition;
}

void Camera::setRotation(vector<float> newRotation) {
    rotation[0] = newRotation[0];
    rotation[1] = newRotation[1];
    rotation[2] = newRotation[2];
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
    rotation[0] = -(sf::Mouse::getPosition(window).x - Engine3D::getWindowSize() / 2) / Engine3D::getWindowSize() * 10 * 180 / M_PI;
    rotation[1] = -(sf::Mouse::getPosition(window).y - Engine3D::getWindowSize() / 2) / Engine3D::getWindowSize() * 10 * 180 / M_PI;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        position.x += engineConf::movementSpeed * direction.normalized().x;
        position.z += engineConf::movementSpeed * direction.normalized().z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        position.x += engineConf::movementSpeed * Engine3D::rotate(direction, {90, 0, 0}).normalized().x;
        position.z += engineConf::movementSpeed * Engine3D::rotate(direction, {90, 0, 0}).normalized().z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        position.x -= engineConf::movementSpeed * direction.normalized().x;
        position.z -= engineConf::movementSpeed * direction.normalized().z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        position.x += engineConf::movementSpeed * Engine3D::rotate(direction, {-90, 0, 0}).normalized().x;
        position.z += engineConf::movementSpeed * Engine3D::rotate(direction, {-90, 0, 0}).normalized().z;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        position.y += engineConf::movementSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        position.y -= engineConf::movementSpeed;
    }

    direction = Engine3D::rotate({0, 0, -1}, rotation);
}
