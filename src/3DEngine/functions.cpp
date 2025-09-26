#include "functions.hpp"
#include <cmath>


float angleBetween(sf::Vector3f vectorA, sf::Vector3f vectorB) {  // returns an absolute value of an angle between two vectors in radians, 3D version
    return acos(vectorA.dot(vectorB) / vectorA.length() / vectorB.length());
}

float angleBetween(sf::Vector2f vectorA, sf::Vector2f vectorB) {  // returns an absolute value of an angle between two vectors in radians, 2D version
    return acos(vectorA.dot(vectorB) / vectorA.length() / vectorB.length());
}

float distanceBetween(sf::Vector2f pointA, sf::Vector2f pointB) {
    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2));
}

float distanceBetween(sf::Vector3f pointA, sf::Vector3f pointB) {
    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2) + pow(pointA.z - pointB.z, 2));
}

