#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace std;

class Object3D {
    float scale;

    sf::Vector3f position;
    sf::Vector3f rotation; // Euler vector where its magnitude is the angle in radians

    vector<sf::Vector3f> points;

    vector<vector<unsigned int>> faces; // an array of sets of 3 indexies of points that make faces

    bool enabled;
public:
    Object3D();
    Object3D(vector<sf::Vector3f> points, vector<vector<unsigned int>> faces, bool enabled);
    Object3D(vector<sf::Vector3f> points, vector<vector<unsigned int>> faces);

    void setScale(float newScale);
    void setPosition(sf::Vector3f newPosition);
    void setRotation(sf::Vector3f newRotation);
    void enable();
    void disable();

    float getScale() const;
    sf::Vector3f getPosition() const;
    sf::Vector3f getRotation() const;
    vector<sf::Vector3f> getPoints() const;
    vector<vector<unsigned int>> getFaces() const;
    bool isEnabled();
};