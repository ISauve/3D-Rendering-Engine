#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include "Glad.h"
#include <iostream>

enum Direction {FORWARD, BACKWARD, LEFT, RIGHT};

class Scene;
class Camera {
    /********* Configurable settings *********/
    const float SCREEN_H = 600.0f;
    const float SCREEN_W = 600.0f;
    const float HEIGHT = 0.8f;
    const float MOUSE_SENSITIVITY = 0.15f;
    const float MOVEMENT_SPEED = 0.02f;
    /*****************************************/

    // Pointer to the scene (used for loading the current terrain)
    Scene* _scene;

    // Camera position & orientation
    glm::vec3 _position;   // Camera position
    glm::vec3 _facing;     // Direction camera is facing (default = -Z)
    glm::vec3 _up;         // World "up" direction (Y axis)

    // Camera state
    float _zoom;
    GLfloat _jumpTime;

    // Euler angles
    GLfloat _yaw;          // Rotation to the left or right
    GLfloat _pitch;        // Rotation up or down

    // Mouse information
    double _xpos;
    double _ypos;
    GLfloat _mouseSensitivity;

public:
    Camera(double, double, Scene*);

    // Accessors
    glm::mat4 ViewMatrix();
    glm::mat4 ProjMatrix();
    glm::vec3 Position();

    // Modifiers
    void Look(double, double);
    void Move(Direction);
    void Zoom(float);

    // Temporary actions
    void Jump();

    // Timer update
    void Tick();
};

#endif
