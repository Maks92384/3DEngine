#include "Engine3D.hpp"
#include <cmath>
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

float Engine3D::windowSize = min(conf::window_size_f.x, conf::window_size_f.y);


void Engine3D::makeNewObject(string name, vector<sf::Vector3f> points, vector<vector<unsigned int>> faces, bool enabled) {
    objects.try_emplace(name, Object3D(std::move(points), std::move(faces), enabled));
}

void Engine3D::makeNewObject(string name, vector<sf::Vector3f> points, vector<vector<unsigned int>> faces) {
    objects.emplace(name, Object3D(std::move(points), std::move(faces)));
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
            sf::Vector3f vectorAC = pointC - pointA;

            normals.push_back({vectorAB.y * vectorAC.z - vectorAB.z * vectorAC.y, vectorAB.z * vectorAC.x - vectorAB.x * vectorAC.z, vectorAB.x * vectorAC.y - vectorAB.y * vectorAC.x});

            sf::Vector3f faceCenter = {(pointA.x + pointB.x + pointC.x) / 3, (pointA.y + pointB.y + pointC.y) / 3, (pointA.z + pointB.z + pointC.z) / 3};

            const sf::Vector2f point2D_A = pointsPositions[face[0]];
            const sf::Vector2f point2D_B = pointsPositions[face[1]];
            const sf::Vector2f point2D_C = pointsPositions[face[2]];

            bool pointAOnScreen = point2D_A.x > 0 && point2D_A.y > 0 && point2D_A.x < conf::window_size_f.x && point2D_A.y < conf::window_size_f.y;
            bool pointBOnScreen = point2D_B.x > 0 && point2D_B.y > 0 && point2D_B.x < conf::window_size_f.x && point2D_B.y < conf::window_size_f.y;
            bool pointCOnScreen = point2D_C.x > 0 && point2D_C.y > 0 && point2D_C.x < conf::window_size_f.x && point2D_C.y < conf::window_size_f.y;

            bool faceOnScreen = pointAOnScreen || pointBOnScreen || pointCOnScreen;

            sf::Vector3f directionToFace = {faceCenter - Camera::getPosition()};

            bool isTheFrontOfTheFace = abs(angleBetween(directionToFace, normals.back())) < 90;


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
    /*
    if (distances.size() >= 2) {
        for (unsigned int i = 0; i < distances.size() - 1; i++) {
            for (unsigned int j = i + 1; j < distances.size(); j++) {
                if (distances[i] < distances[j]) {
                    float temp = distances[i];
                    distances[i] = distances[j];
                    distances[j] = temp;

                    vector<sf::Vector2f> temp2 = facesToDraw[i];
                    facesToDraw[i] = facesToDraw[j];
                    facesToDraw[j] = temp2;
                }
            }
        }
    }
    */

    for (unsigned int i = 0; i < facesToDraw.size(); i++) {
        sf::ConvexShape face(3);

        face.setPoint(0, facesToDraw[i][0]);
        face.setPoint(1, facesToDraw[i][1]);
        face.setPoint(2, facesToDraw[i][2]);

        face.setFillColor(sf::Color(255, 100, 0));

        window.draw(face);


        sf::VertexArray outline(sf::PrimitiveType::LineStrip, 4);

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
        // positive XYZ lines

        sf::VertexArray Axes(sf::PrimitiveType::Lines, 6);

        Axes[0].position = transform({0, 0, 0});
        Axes[1].position = transform({50, 0, 0});
        Axes[0].color = sf::Color(255, 0, 0);
        Axes[1].color = sf::Color(255, 0, 0);

        Axes[2].position = transform({0, 0, 0});
        Axes[3].position = transform({0, 50, 0});
        Axes[2].color = sf::Color(0, 255, 0);
        Axes[3].color = sf::Color(0, 255, 0);

        Axes[4].position = transform({0, 0, 0});
        Axes[5].position = transform({0, 0, 50});
        Axes[4].color = sf::Color(0, 0, 255);
        Axes[5].color = sf::Color(0, 0, 255);

        window.draw(Axes);

        // World center

        sf::CircleShape worldCenter(5);

        worldCenter.setPosition(transform({0, 0, 0}));

        worldCenter.setFillColor(sf::Color(255, 0, 0));
        worldCenter.setOrigin({5, 5});

        window.draw(worldCenter);
    }
}

sf::Vector2f Engine3D::transform(sf::Vector3f point3D) {
    sf::Vector3f xAxis = {1, 0, 0};
    sf::Vector3f yAxis = {0, 1, 0};
    sf::Vector3f zAxis = {0, 0, 1};

    sf::Vector3f vectorToTransform = point3D - Camera::getPosition();

    vectorToTransform = unRotate(vectorToTransform, Camera::getRotation());

    float copyOfTheZValue = vectorToTransform.z;


    vectorToTransform.z = 0;

    float rollAngle = angleBetween(yAxis, vectorToTransform);

    if (vectorToTransform.x == 0 && vectorToTransform.y == 0)
        rollAngle = 0;

    if (angleBetween(-xAxis, vectorToTransform) > 90)
        rollAngle *= -1;

    vectorToTransform.z = copyOfTheZValue;


    sf::Vector3f transformedVector = {0, angleBetween(-zAxis, vectorToTransform) / (90 * Camera::getFov() / 180), 0};

    transformedVector = rotate(transformedVector, {0, 0, rollAngle});

    return {max(conf::window_size_f.x, conf::window_size_f.y) / 2 * (1 + transformedVector.x), max(conf::window_size_f.x, conf::window_size_f.y) / 2 * (1 - transformedVector.y)};
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

float Engine3D::getWindowSize() {
    return windowSize;
}
