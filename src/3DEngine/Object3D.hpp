#pragma once
#include <vector>

#include "SFML/System/Vector3.hpp"

using namespace std;

class Object3D {
    float scale;

    sf::Vector3f position;
    vector<float> rotation;

    vector<sf::Vector3f> points;

    vector<vector<unsigned int>> faces; // an array of sets of 3 indexies of points that make faces

    bool enabled;
public:
    Object3D();
    Object3D(vector<sf::Vector3f> points, vector<vector<unsigned int>> faces, bool enabled);
    Object3D(vector<sf::Vector3f> points, vector<vector<unsigned int>> faces);

    void setScale(float newScale);
    void setPosition(sf::Vector3f newPosition);
    void setRotation(vector<float> newRotation);
    void enable();
    void disable();

    float getScale() const;
    sf::Vector3f getPosition() const;
    vector<float> getRotation() const;
    vector<sf::Vector3f> getPoints() const;
    vector<vector<unsigned int>> getFaces() const;
    bool isEnabled();
};