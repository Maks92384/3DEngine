#include "functions.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>


float angleBetween(sf::Vector3f vectorA, sf::Vector3f vectorB) {  // returns an absolute value of an angle between two vectors, 3D version
    return acos(vectorA.dot(vectorB) / vectorA.length() / vectorB.length()) * 180 / M_PI;
}

float angleBetween(sf::Vector2f vectorA, sf::Vector2f vectorB) {  // returns an absolute value of an angle between two vectors, 2D version
    return acos(vectorA.dot(vectorB) / vectorA.length() / vectorB.length()) * 180 / M_PI;
}

float distanceBetween(sf::Vector2f pointA, sf::Vector2f pointB) {
    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2));
}

float distanceBetween(sf::Vector3f pointA, sf::Vector3f pointB) {
    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2) + pow(pointA.z - pointB.z, 2));
}

void radixSortForFaces(vector<float>& distances, vector<vector<sf::Vector2f>>& faces) {
    vector<array<uint16_t, 2>> distancesAsUint16; // size is the size of distances
    array<uint16_t, 2> tempDistanceAsInt16;

    for (float& distance : distances) {
        tempDistanceAsInt16[0] = *reinterpret_cast<uint32_t*>(&distance) / 65536;
        tempDistanceAsInt16[1] = *reinterpret_cast<uint32_t*>(&distance) % 65536;
        distancesAsUint16.push_back(tempDistanceAsInt16);
    }


    // Sorting

    vector<array<uint16_t, 2>> buckets[65536] = {};
    //static array<vector<vector<sf::Vector2f>>, 65536> faceBuckets;
    vector<vector<vector<sf::Vector2f>>> faceBuckets(65536);

    unsigned int index = 0;
    for (array<uint16_t, 2>& distance : distancesAsUint16) {
        buckets[distance[1]].push_back(distance);
        faceBuckets[distance[1]].push_back(faces[index]);
        index++;
    }

    distancesAsUint16.clear();
    faces.clear();

    index = 0;
    for (vector<array<uint16_t, 2>>& bucket : buckets) {
        for (uint16_t i = 0; i < bucket.size(); i++) {
            distancesAsUint16.push_back(bucket[i]);
            faces.push_back(faceBuckets[index][i]);
        }
        index++;
    }

    index = 0;
    for (vector<array<uint16_t, 2>>& bucket : buckets) {
        bucket.clear();
        faceBuckets[index].clear();
        index++;
    }

    index = 0;
    for (array<uint16_t, 2>& distance : distancesAsUint16) {
        buckets[distance[0]].push_back(distance);
        faceBuckets[distance[0]].push_back(faces[index]);
        index++;
    }

    faces.clear();

    index = 0;
    for (vector<array<uint16_t, 2>>& bucket : buckets) {
        for (uint16_t i = 0; i < bucket.size(); i++) {
            faces.push_back(faceBuckets[index][i]);
        }
        index++;
    }

    reverse(faces.begin(), faces.end());
}