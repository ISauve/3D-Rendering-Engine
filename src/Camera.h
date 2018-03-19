#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

enum Direction {FORWARD, BACKWARD, LEFT, RIGHT};

class Camera {
    // Camera position & orientation
    glm::vec3 _position;   // Camera position
    glm::vec3 _facing;     // Direction camera is facing (default = -Z)
    glm::vec3 _up;         // World "up" direction (Y axis)

    GLfloat _height;
    GLfloat _jumpTime;

    // Euler angles
    GLfloat _yaw;          // Rotation to the left or right
    GLfloat _pitch;        // Rotation up or down

    // Mouse information
    double _xpos;
    double _ypos;
    GLfloat _mouseSensitivity; // Sensitivity to movement (default = 0.25f)

public:
    Camera(double, double);

    glm::mat4 ViewMatrix();
    glm::mat4 ProjMatrix();
    glm::vec3 Position();

    void Move(Direction);
    void Look(double xoffset, double yoffset);

    void isDucking(bool);
    void Jump();
    void Tick();

    void getID() {
        std::cout << "Camera " << this << std::endl;
    }
};

#endif
