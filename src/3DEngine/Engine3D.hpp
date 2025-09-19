#pragma once
#include "Object3D.hpp"
#include "SFML/Graphics.hpp"

using namespace std;

class Engine3D {
    static unordered_map<string, Object3D> objects;
    static vector<string> objectsToDraw;

    static vector<sf::Vector2f> pointsToDraw;
    static vector<vector<sf::Vector2f>> facesToDraw;
    static vector<float> distances;

    static sf::Vector2f transform(sf::Vector3f point);

    static void radixSortForFaces(vector<float>& distances, vector<vector<sf::Vector2f>>& faces);
public:
    static sf::Vector3f rotate(sf::Vector3f initialDirection, vector<float> rotation);
    static sf::Vector3f unRotate(sf::Vector3f initialDirection, vector<float> rotation);

    static void render();
    static void draw(sf::RenderWindow& window);
    static void makeNewObject(string name, vector<sf::Vector3f> points, vector<vector<unsigned int>> faces, bool enabled);
    static void makeNewObject(string name, vector<sf::Vector3f> points, vector<vector<unsigned int>> faces);

    static void generateBox(string name, sf::Vector3f position, sf::Vector3i dimensions);
    static void loadFromFile(string path, string name);

    static float getWindowSize();
};
