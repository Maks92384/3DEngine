#include "Object3D.hpp"
#include "engineConfiguration.hpp"

Object3D::Object3D() = default;

Object3D::Object3D(vector<sf::Vector3f> points, vector<vector<unsigned int>> faces, bool enabled) : points(points), faces(faces), enabled(enabled), scale(engineConf::objectScale), position({0, 0, 0}), rotation(3, 0.0f) {}

Object3D::Object3D(vector<sf::Vector3f> points, vector<vector<unsigned int>> faces) : Object3D(points, faces, false) {}



void Object3D::setScale(float newScale) {
    scale = newScale;
}

void Object3D::setPosition(sf::Vector3f newPosition) {
    position = newPosition;
}

void Object3D::setRotation(vector<float> newRotation) {
    rotation = newRotation;
}

void Object3D::enable() {
    enabled = true;
}

void Object3D::disable() {
    enabled = false;
}


float Object3D::getScale() const  {
    return scale;
}

sf::Vector3f Object3D::getPosition() const  {
    return position;
}

vector<float> Object3D::getRotation() const  {
    return rotation;
}

vector<sf::Vector3f> Object3D::getPoints() const {
    return points;
}

vector<vector<unsigned int>> Object3D::getFaces() const  {
    return faces;
}

bool Object3D::isEnabled() {
    return enabled;
}