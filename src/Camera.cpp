#include "Camera.h"
#include "Objects/Object.h"
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera(double xpos, double ypos, Scene* s) : _scene(s),
        _position(vec3(0.0f, HEIGHT, 3.0f)),  _facing(vec3(0.0f, 0.0f, -1.0f)), _up(vec3(0.0f, 1.0f, 0.0f)),
        _zoom(45.0f), _jumpTime(-1.0f),
        _yaw(-90.0f), _pitch(0.0f),
        _xpos(xpos), _ypos(ypos), _mouseSensitivity(MOUSE_SENSITIVITY) {

    try {
        float terrainH = _scene->currTerrain()->getHeightAt(_position.x, _position.z);
        _position.y = terrainH + HEIGHT;
    } catch (std::runtime_error& e) {
        if (DEBUG) std::cerr << "Warning: camera initialized without a terrain.\n";
        _position.y = HEIGHT;
    }
}

mat4 Camera::ViewMatrix() {
    return lookAt(_position, _position + _facing, _up);
}

mat4 Camera::ProjMatrix() {
    float aspectRatio = SCREEN_W / SCREEN_H;
    return perspective(radians(_zoom),
                       aspectRatio,
                       0.1f,            // "near" clipping plane
                       100.0f);         // "far" clipping plane
}

vec3 Camera::Position() {
    return _position;
}

void Camera::Look(double xpos, double ypos) {
    GLfloat xoffset = xpos - _xpos; // x coords increase from left to right
    GLfloat yoffset = _ypos - ypos; // y coords increase from top to bottom
    _xpos = xpos;
    _ypos = ypos;

    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw   += xoffset;
    _pitch += yoffset;

    // Limit the pitch so we don't flip "backwards"
    if (_pitch > 89.0f) _pitch = 89.0f;
    if (_pitch < -89.0f) _pitch = -89.0f;

    // Update the direction we are facing
    _facing.x = cos(radians(_yaw)) * cos(radians(_pitch));
    _facing.y = sin(radians(_pitch));
    _facing.z = sin(radians(_yaw)) * cos(radians(_pitch));
    _facing = normalize(_facing);
}

void Camera::Move(Direction d) {
    switch(d) {
        case FORWARD:
            _position += _facing * MOVEMENT_SPEED;
            break;
        case BACKWARD:
            _position -= _facing * MOVEMENT_SPEED;
            break;
        case LEFT:
            _position -= normalize(cross(_facing, _up)) * MOVEMENT_SPEED;
            break;
        case RIGHT:
            _position += normalize(cross(_facing, _up)) * MOVEMENT_SPEED;
            break;
        default: break;
    }

    try {
        float terrainH = _scene->currTerrain()->getHeightAt(_position.x, _position.z);
        _position.y = terrainH + HEIGHT;
    } catch (std::runtime_error& e) {
        if (DEBUG) std::cerr << "Warning: terrain is null, camera height = default height.\n";
        _position.y = HEIGHT;
    }
}

void Camera::Zoom(float yoffset) {
    _zoom -= yoffset;
    if (_zoom < 1.0f) _zoom = 1.0f;
    if (_zoom > 45.0f) _zoom = 45.0f;
}

void Camera::Jump() {
    // Start a jump (if we're not currently in one)
    if (_jumpTime == -1) _jumpTime = 0;
}

// This fcn gets called approximately once every 0.016 seconds (1/60 FPS)
void Camera::Tick() {
    if (_jumpTime == -1) return;  // don't do anything if we're not jumping

    float terrainH = 0;
    try {
        terrainH = _scene->currTerrain()->getHeightAt(_position.x, _position.z);
    } catch (std::runtime_error& e) {
        if (DEBUG) std::cerr << "Warning: terrain is null, jump height ignores terrain height.\n";
    }

    float jumpH = 3.0f * _jumpTime + 0.5f * -9.8f * _jumpTime * _jumpTime;
    _position.y = terrainH + jumpH + HEIGHT;
    _jumpTime += 0.016;

    if (jumpH < 0) {     // jump is finished
        _position.y = terrainH + HEIGHT;
        _jumpTime = -1;
    }
}
