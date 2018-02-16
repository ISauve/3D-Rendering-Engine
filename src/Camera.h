#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Direction {FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN};

class Camera {
    // Camera position & orientation
    glm::vec3 _position;   // Camera position
    glm::vec3 _facing;     // Direction camera is facing (default = -Z)
    glm::vec3 _up;         // World "up" direction (default = Y axis)

    // Euler angles
    GLfloat _yaw;          // Rotation to the left or right
    GLfloat _pitch;        // Rotation up or down

public:
    Camera();

    glm::mat4 ViewMatrix();

    void Move(Direction);
    void Look(Direction);
};


#endif
