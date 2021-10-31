#ifndef CAMERA_H
#define CAMERA_H

#include <deque>
#include <vector>
#include <math.h>
#include <string>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "Object.h"

#define PI 3.141592653

/*
We work in a right-handed axis system.
In camera space, the positive z-axis points in the direction of the camera.
The x-axis is left and the y-axis is up.
In screen space, these coordinates are inverted.
*/

class Camera
{
private:
    Eigen::Matrix4f getTranslation();
    Eigen::Matrix4f worldSpaceToCameraSpace();
    Eigen::Matrix4f cameraSpaceToClipSpace();

public:
    sf::RenderTexture render_texture;
    int width, height;
    std::deque<Object> myObjects;
    sf::Color background_color;

    float zoom_x, zoom_y;
    float near_z, far_z;

    float x,y,z;
    float speed;
    Eigen::Vector3f angle; //a normalized vector that indicates the camera's viewing direction
    float maxLookUp;

    Camera(int width_, int height_, int x, int y, int z, std::deque<Object> objects);
    void changePos (float dx, float dy, float dz);
    void strafe (float right, float up, float forw);
    void rotateAngle (float rotateToX, float rotateToY);
    void display();
    void displayCoordinateSystems();
};

#endif // CAMERA_H
