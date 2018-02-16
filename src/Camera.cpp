#include "Camera.h"

using namespace glm;

Camera::Camera() {
    _position = vec3(0.0f, 0.0f, 1.0f);
    _facing = vec3(0.0f, 0.0f, -1.0f);
    _up = vec3(0.0f, 1.0f, 0.0f);
    _yaw = -90.0f;
    _pitch = 0.0f;
}

mat4 Camera::ViewMatrix() {
    return lookAt(_position, _position + _facing, _up);
}

void Camera::Move(Direction d) {
    float dt = 0.01f;

    switch(d) {
        case FORWARD:
            _position += _facing * dt;
            break;
        case BACKWARD:
            _position -= _facing * dt;
            break;
        case LEFT:
            _position -= normalize(cross(_facing, _up)) * dt;
            break;
        case RIGHT:
            _position += normalize(cross(_facing, _up)) * dt;
            break;
        default: break;
    }
}

void Camera::Look(Direction d) {
    float theta = 1.0f;

    switch(d) {
        case UP:
            _pitch += theta;
            break;
        case DOWN:
            _pitch -= theta;
            break;
        case LEFT:
            _yaw -= theta;
            break;
        case RIGHT:
            _yaw += theta;
            break;
        default: break;
    }

    // Limit the pitch so we don't flip "backwards"
    if (_pitch > 89.0f) _pitch = 89.0f;
    if (_pitch < -89.0f) _pitch = -89.0f;

    // Update the directional vectors
    _facing.x = cos(radians(_yaw)) * cos(radians(_pitch));
    _facing.y = sin(radians(_pitch));
    _facing.z = sin(radians(_yaw)) * cos(radians(_pitch));
    _facing = normalize(_facing);

    vec3 right = normalize(cross(_facing, _up));
    _up = normalize(cross(right, _facing));
}