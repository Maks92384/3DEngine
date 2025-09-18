#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

float angleBetween(sf::Vector2f vectorA, sf::Vector2f vectorB);

float angleBetween(sf::Vector3f vectorA, sf::Vector3f vectorB);

float distanceBetween(sf::Vector2f pointA, sf::Vector2f pointB);

float distanceBetween(sf::Vector3f pointA, sf::Vector3f pointB);

void radixSortForFaces(vector<float>& distances, vector<vector<sf::Vector2f>>& faces);