#include "Engine3D.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include "Camera.hpp"
#include "engineConfiguration.hpp"
#include "functions.hpp"
#include "../configuration.hpp"

unordered_map<string, Object3D> Engine3D::objects;
vector<string> Engine3D::objectsToDraw;

vector<sf::Vector2f> Engine3D::pointsToDraw;
vector<vector<sf::Vector2f>> Engine3D::facesToDraw;
vector<float> Engine3D::distances;


void Engine3D::makeNewObject(string name, vector<sf::Vector3f> points, vector<vector<unsigned int>> faces, bool enabled) {
    for (unsigned int i = 0 ; i < faces.size(); i++) {
        if (faces[i].size() >= 4) {
            vector<unsigned int> face_copy = faces[i];
            face_copy.erase(face_copy.begin() + 1);
            faces.push_back(face_copy);
            faces[i].resize(3);
        }
    }

    objects[name] = Object3D(points, faces, enabled);
}

void Engine3D::makeNewObject(string name, vector<sf::Vector3f> points, vector<vector<unsigned int>> faces) {
    makeNewObject(name, points, faces, false);
}


sf::Vector3f Engine3D::rotate(sf::Vector3f initialDirection, vector<float> rotation) {
    float yaw = rotation[0] * M_PI / 180;
    float pitch = rotation[1] * M_PI / 180;
    float roll = rotation[2] * M_PI / 180;

    float rotationMatrix[3][3] = {
        {cos(yaw) * cos(roll) + sin(yaw) * sin(pitch) * sin(roll), cos(roll) * sin(yaw) * sin(pitch) - cos(yaw) * sin(roll), cos(pitch) * sin(yaw)},
        {cos(pitch) * sin(roll), cos(pitch) * cos(roll), -sin(pitch)},
        {cos(yaw) * sin(pitch) * sin(roll) - cos(roll) * sin(yaw), cos(yaw) * cos(roll) * sin(pitch) + sin(yaw) * sin(roll), cos(yaw) * cos(pitch)}
    };

    sf::Vector3f rotatedDirection;

    rotatedDirection.x = initialDirection.x * rotationMatrix[0][0] + initialDirection.y * rotationMatrix[0][1] + initialDirection.z * rotationMatrix[0][2];
    rotatedDirection.y = initialDirection.x * rotationMatrix[1][0] + initialDirection.y * rotationMatrix[1][1] + initialDirection.z * rotationMatrix[1][2];
    rotatedDirection.z = initialDirection.x * rotationMatrix[2][0] + initialDirection.y * rotationMatrix[2][1] + initialDirection.z * rotationMatrix[2][2];

    return rotatedDirection;
}

sf::Vector3f Engine3D::unRotate(sf::Vector3f initialDirection, vector<float> rotation) {
    float yaw = -rotation[0] * M_PI / 180;
    float pitch = -rotation[1] * M_PI / 180;
    float roll = -rotation[2] * M_PI / 180;

    float rotationMatrix[3][3] = {
      {cos(roll) * cos(yaw) - sin(roll) * sin(pitch) * sin(yaw), -cos(pitch) * sin(roll), cos(roll) * sin(yaw) + cos(yaw) * sin(roll) * sin(pitch)},
      {cos(yaw) * sin(roll) + cos(roll) * sin(pitch) * sin(yaw), cos(roll) * cos(pitch), sin(roll) * sin(yaw) - cos(roll) * cos(yaw) * sin(pitch)},
      {-cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw)}
    };

    sf::Vector3f rotatedDirection;

    rotatedDirection.x = initialDirection.x * rotationMatrix[0][0] + initialDirection.y * rotationMatrix[0][1] + initialDirection.z * rotationMatrix[0][2];
    rotatedDirection.y = initialDirection.x * rotationMatrix[1][0] + initialDirection.y * rotationMatrix[1][1] + initialDirection.z * rotationMatrix[1][2];
    rotatedDirection.z = initialDirection.x * rotationMatrix[2][0] + initialDirection.y * rotationMatrix[2][1] + initialDirection.z * rotationMatrix[2][2];

    return rotatedDirection;
}


void Engine3D::render() {
    for (auto& [objectName, object] : objects) {
        if (!object.isEnabled()) break;

        bool drawObject = true;

        for (string objectToDraw : objectsToDraw) {
            if (objectName == objectToDraw)
                drawObject = false;
        }

        vector<sf::Vector2f> pointsPositions;

        vector<sf::Vector3f> rotatedPoints = {};

        for (const auto& point : object.getPoints()) {
            rotatedPoints.push_back(rotate(point, object.getRotation()));

            rotatedPoints.back().x = rotatedPoints.back().x * object.getScale() + object.getPosition().x;
            rotatedPoints.back().y = rotatedPoints.back().y * object.getScale() + object.getPosition().y;
            rotatedPoints.back().z = rotatedPoints.back().z * object.getScale() + object.getPosition().z;

            pointsPositions.push_back(transform(rotatedPoints.back()));

            if (drawObject)
                pointsToDraw.push_back(pointsPositions.back());
        }

        vector<sf::Vector3f> normals;

        for (auto& face : object.getFaces()) {
            const sf::Vector3f pointA = rotatedPoints[face[0]];
            const sf::Vector3f pointB = rotatedPoints[face[1]];
            const sf::Vector3f pointC = rotatedPoints[face[2]];

            sf::Vector3f vectorAB = pointB - pointA;
            sf::Vector3f vectorBC = pointB - pointC;

            normals.push_back({vectorAB.y * vectorBC.z - vectorAB.z * vectorBC.y, vectorAB.z * vectorBC.x - vectorAB.x * vectorBC.z, vectorAB.x * vectorBC.y - vectorAB.y * vectorBC.x});

            sf::Vector3f faceCenter = {(pointA.x + pointB.x + pointC.x) / 3, (pointA.y + pointB.y + pointC.y) / 3, (pointA.z + pointB.z + pointC.z) / 3};

            const sf::Vector2f point2D_A = pointsPositions[face[0]];
            const sf::Vector2f point2D_B = pointsPositions[face[1]];
            const sf::Vector2f point2D_C = pointsPositions[face[2]];

            bool pointAOnScreen = point2D_A.x > 0 && point2D_A.y > 0 && point2D_A.x < conf::window_size_f.x && point2D_A.y < conf::window_size_f.y;
            bool pointBOnScreen = point2D_B.x > 0 && point2D_B.y > 0 && point2D_B.x < conf::window_size_f.x && point2D_B.y < conf::window_size_f.y;
            bool pointCOnScreen = point2D_C.x > 0 && point2D_C.y > 0 && point2D_C.x < conf::window_size_f.x && point2D_C.y < conf::window_size_f.y;

            bool faceOnScreen = pointAOnScreen || pointBOnScreen || pointCOnScreen;

            sf::Vector3f directionToFace = {faceCenter - Camera::getPosition()};

            bool isTheFrontOfTheFace = abs(angleBetween(directionToFace, normals.back())) <= 90;


            if (faceOnScreen && isTheFrontOfTheFace && drawObject) {
                facesToDraw.push_back({point2D_A, point2D_B, point2D_C});
                distances.push_back(distanceBetween(Camera::getPosition(), faceCenter));
            }
        }

        if (drawObject)
            objectsToDraw.push_back(objectName);
    }
}

void Engine3D::draw(sf::RenderWindow& window) {
    radixSortForFaces(distances, facesToDraw);

    sf::ConvexShape face(3);
    face.setFillColor(sf::Color(255, 100, 0));
    sf::VertexArray outline(sf::PrimitiveType::LineStrip, 4);

    sf::Color outlineColor(70, 70, 70);

    outline[0].color = outlineColor;
    outline[1].color = outlineColor;
    outline[2].color = outlineColor;
    outline[3].color = outlineColor;

    for (unsigned int i = 0; i < facesToDraw.size(); i++) {
        face.setPoint(0, facesToDraw[i][0]);
        face.setPoint(1, facesToDraw[i][1]);
        face.setPoint(2, facesToDraw[i][2]);


        window.draw(face);

        outline[0].position = facesToDraw[i][0];
        outline[1].position = facesToDraw[i][1];
        outline[2].position = facesToDraw[i][2];
        outline[3].position = facesToDraw[i][0];

        window.draw(outline);
    }

    /*
    for (unsigned int i = 0; i < pointsToDraw.size(); i++) {
        sf::CircleShape point(3);

        point.setPosition(pointsToDraw[i]);

        point.setFillColor(sf::Color(0, 0, 0));
        point.setOrigin({3, 3});

        window.draw(point);
    }
    */

    objectsToDraw.clear();
    pointsToDraw.clear();
    facesToDraw.clear();
    distances.clear();

    // Debug Mode

    if (engineConf::debugMode) {
        // World center

        sf::CircleShape worldCenter(5);

        worldCenter.setPosition(transform({0, 0, 0}));

        worldCenter.setFillColor(sf::Color(255, 0, 0));
        worldCenter.setOrigin({5, 5});

        window.draw(worldCenter);


        // positive XYZ lines

        sf::VertexArray Axes(sf::PrimitiveType::Lines, 6);

        Axes[0].position = conf::window_size_f / 2.0f;
        Axes[1].position = conf::window_size_f / 2.0f + sf::Vector2f(unRotate({50, 0, 0}, Camera::getRotation()).x, unRotate({50, 0, 0}, Camera::getRotation()).y);
        Axes[0].color = sf::Color(255, 0, 0);
        Axes[1].color = sf::Color(255, 0, 0);

        Axes[2].position = conf::window_size_f / 2.0f;
        Axes[3].position = conf::window_size_f / 2.0f - sf::Vector2f(unRotate({0, 50, 0}, Camera::getRotation()).x, unRotate({0, 50, 0}, Camera::getRotation()).y);
        Axes[2].color = sf::Color(0, 255, 0);
        Axes[3].color = sf::Color(0, 255, 0);

        Axes[4].position = conf::window_size_f / 2.0f;
        Axes[5].position = conf::window_size_f / 2.0f + sf::Vector2f(unRotate({0, 0, 50}, Camera::getRotation()).x, unRotate({0, 0, 50}, Camera::getRotation()).y);
        Axes[4].color = sf::Color(0, 0, 255);
        Axes[5].color = sf::Color(0, 0, 255);

        window.draw(Axes);
    }
}

sf::Vector2f Engine3D::transform(sf::Vector3f point3D) {
    float fovInRadians = Camera::getFov() * M_PI / 180;

    point3D -= Camera::getPosition();

    point3D = unRotate(point3D, Camera::getRotation());

    float pointDistance = -point3D.z;

    sf::Vector2f scaledCameraSize = {tan(fovInRadians / 2) * pointDistance * 2, tan(fovInRadians / 2) * conf::window_size_f.y / conf::window_size_f.x * pointDistance * 2};

    sf::Vector2f transformedPoint = {point3D.x * conf::window_size_f.x / scaledCameraSize.x, point3D.y * conf::window_size_f.y / scaledCameraSize.y};

    if (pointDistance < 0) {
        transformedPoint.x *= 1000000;
        transformedPoint.y *= 1000000;
    }

    return {conf::window_size_f.x / 2 + transformedPoint.x , conf::window_size_f.y / 2  - transformedPoint.y};
}

void Engine3D::generateBox(string name, sf::Vector3f position, sf::Vector3i dimensions /* (x=Length, y=Height, z=Depth) */ ) {
    vector<sf::Vector3f> points;
    vector<vector<unsigned int>> faces;

    vector<vector<vector<int>>> grid;

    for (unsigned int i = 0; i <= abs(dimensions.x); i++) {
        vector<vector<int>> gridSlice;

        for (unsigned int j = 0; j <= abs(dimensions.y); j++) {
            vector<int> gridRow;

            for (unsigned int k = 0; k <= abs(dimensions.z); k++) {
                gridRow.push_back(1);
            }

            gridSlice.push_back(gridRow);
        }

        grid.push_back(gridSlice);
    }

    for (unsigned int i = 1; i < abs(dimensions.x); i++) {
        for (unsigned int j = 1; j < abs(dimensions.y); j++) {
            for (unsigned int k = 1; k < abs(dimensions.z); k++) {
                grid[i][j][k] = -1;
            }
        }
    }

    unsigned int counter = 0;

    for (unsigned int k = 0; k <= abs(dimensions.z); k++) {
        for (unsigned int j = 0; j <= abs(dimensions.y); j++) {
            for (unsigned int i = 0; i <= abs(dimensions.x); i++) {
                if (grid[i][j][k] >= 0) {
                    grid[i][j][k] = counter++;
                    points.push_back({(float) i * dimensions.x / abs(dimensions.x), (float) j * dimensions.y / abs(dimensions.y), (float) k * dimensions.z / abs(dimensions.z)});
                }
            }
        }
    }

    for (unsigned int j = 1; j <= abs(dimensions.y); j++) {
        for (unsigned int i = 1; i <= abs(dimensions.x); i++) {
            faces.push_back({(unsigned int) grid[i][j][0], (unsigned int) grid[i - 1][j - 1][0], (unsigned int) grid[i - 1][j][0]});
            faces.push_back({(unsigned int) grid[i][j][0], (unsigned int) grid[i][j - 1][0], (unsigned int) grid[i - 1][j - 1][0]});
            faces.push_back({(unsigned int) grid[i][j][abs(dimensions.z)],  (unsigned int) grid[i - 1][j][abs(dimensions.z)], (unsigned int) grid[i - 1][j - 1][abs(dimensions.z)]});
            faces.push_back({(unsigned int) grid[i][j][abs(dimensions.z)], (unsigned int) grid[i - 1][j - 1][abs(dimensions.z)], (unsigned int) grid[i][j - 1][abs(dimensions.z)]});
        }
    }

    for (unsigned int j = 1; j <= abs(dimensions.z); j++) {
        for (unsigned int i = 1; i <= abs(dimensions.x); i++) {
            faces.push_back({(unsigned int) grid[i][0][j], (unsigned int) grid[i - 1][0][j], (unsigned int) grid[i - 1][0][j - 1]});
            faces.push_back({(unsigned int) grid[i][0][j], (unsigned int) grid[i - 1][0][j - 1], (unsigned int) grid[i][0][j - 1]});
            faces.push_back({(unsigned int) grid[i][abs(dimensions.y)][j], (unsigned int) grid[i - 1][abs(dimensions.y)][j - 1], (unsigned int) grid[i - 1][abs(dimensions.y)][j]});
            faces.push_back({(unsigned int) grid[i][abs(dimensions.y)][j], (unsigned int) grid[i][abs(dimensions.y)][j - 1], (unsigned int) grid[i - 1][abs(dimensions.y)][j - 1]});
        }
    }

    for (unsigned int j = 1; j <= abs(dimensions.z); j++) {
        for (unsigned int i = 1; i <= abs(dimensions.y); i++) {
            faces.push_back({(unsigned int) grid[0][i][j], (unsigned int) grid[0][i - 1][j - 1], (unsigned int) grid[0][i - 1][j]});
            faces.push_back({(unsigned int) grid[0][i][j], (unsigned int) grid[0][i][j - 1], (unsigned int) grid[0][i - 1][j - 1]});
            faces.push_back({(unsigned int) grid[abs(dimensions.x)][i][j], (unsigned int) grid[abs(dimensions.x)][i - 1][j], (unsigned int) grid[abs(dimensions.x)][i - 1][j - 1]});
            faces.push_back({(unsigned int) grid[abs(dimensions.x)][i][j], (unsigned int) grid[abs(dimensions.x)][i - 1][j - 1], (unsigned int) grid[abs(dimensions.x)][i][j - 1]});
        }
    }

    int dimensionsSign = dimensions.x * dimensions.y * dimensions.z / abs(dimensions.x * dimensions.y * dimensions.z);

    if (dimensionsSign == 1) {
        for (auto& face : faces) {
            unsigned int temp = face[0];
            face[0] = face[1];
            face[1] = temp;
        }
    }

    makeNewObject(name, points, faces, true);

    objects[name].setPosition(position);
}

void Engine3D::radixSortForFaces(vector<float>& distances, vector<vector<sf::Vector2f>>& faces) {
    vector<array<uint16_t, 2>> distancesAsUint16; // size is the size of distances
    array<uint16_t, 2> tempDistanceAsInt16;

    for (float& distance : distances) {
        tempDistanceAsInt16[0] = *reinterpret_cast<uint32_t*>(&distance) / 65536;
        tempDistanceAsInt16[1] = *reinterpret_cast<uint32_t*>(&distance) % 65536;
        distancesAsUint16.push_back(tempDistanceAsInt16);
    }


    // Sorting

    vector<array<uint16_t, 2>> buckets[65536] = {};
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

void Engine3D::loadFromFile(string fileName, string objectName) {
    fstream file;
    file.open("../../Models/" + fileName);

    if (!file) {
        cout<<"Unable to open file: " + fileName<<endl;
        return;
    }

    vector<sf::Vector3f> points;
    vector<vector<unsigned int>> faces;

    string line;

    while (getline(file, line)) {
        string numberAsString = "";
        vector<float> point;
        vector<unsigned int> face;

        bool wait = false;

        if (line.size() >= 2) {
            if (line[0] == 'v' && line[1] == ' ') {
                for (unsigned int i = 2; i < line.size(); i++) {
                    if (line[i] == ' ') {
                    point.push_back(stof(numberAsString));
                    numberAsString = "";
                    } else {
                    numberAsString += line[i];
                    }
                }
                point.push_back(stof(numberAsString));
                numberAsString = "";

                if (point.size() >= 3) {
                    points.push_back(sf::Vector3f(point[0], point[1], point[2]));
                }
            }

            numberAsString = "";

            if (line[0] == 'f' && line[1] == ' ') {
                for (unsigned int i = 2; i < line.size(); i++) {
                    if (line[i] == ' ') {
                        face.push_back(stoi(numberAsString) - 1);
                        numberAsString = "";
                        wait = false;
                    } else if (line[i] == '/') {
                        wait = true;
                    } else if (!wait) {
                        numberAsString += line[i];
                    }
                }
                face.push_back(stoi(numberAsString) - 1);
                numberAsString = "";

                if (face.size() >= 3) {
                    faces.push_back(face);
                }
            }
        }
    }

    file.close();

    makeNewObject(objectName, points, faces, true);
}