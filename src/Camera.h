#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

enum Direction {FORWARD, BACKWARD, LEFT, RIGHT};

class Terrain;
class Camera {
    /********* Configurable settings *********/
    const float SCREEN_H = 600.0f;
    const float SCREEN_W = 600.0f;
    const float HEIGHT = 0.8f;
    const float MOUSE_SENSITIVITY = 0.15f;
    const float MOVEMENT_SPEED = 0.02f;
    /*****************************************/

    // Camera position & orientation
    glm::vec3 _position;   // Camera position
    glm::vec3 _facing;     // Direction camera is facing (default = -Z)
    glm::vec3 _up;         // World "up" direction (Y axis)

    // Camera state
    float _zoom;
    bool _ducking;
    GLfloat _height;
    GLfloat _jumpTime;

    // Euler angles
    GLfloat _yaw;          // Rotation to the left or right
    GLfloat _pitch;        // Rotation up or down

    // Mouse information
    double _xpos;
    double _ypos;
    GLfloat _mouseSensitivity;

    // Pointer to terrain we're currently standing on
    Terrain* _currTerrain;

public:
    Camera(double, double);

    // Accessors
    glm::mat4 ViewMatrix();
    glm::mat4 ProjMatrix();
    glm::vec3 Position();

    // Modifiers
    void Look(double, double);
    void Move(Direction);
    void Zoom(float);
    void setCurrTerrain(Terrain* t) { _currTerrain = t; };

    // Temporary actions
    void isDucking(bool);
    void Jump();

    // Timer update
    void Tick();
};

#endif
