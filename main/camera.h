// Reference : https://learnopengl.com/
#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad.h>
#include <vector>

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#define PI acos(-1)

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    Vec3f Position;
    Vec3f Front;
    Vec3f Up;
    Vec3f Right;
    Vec3f WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(Vec3f position = Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f up = Vec3f{ 0.0f, 1.0f, 0.0f }, float yaw = YAW, float pitch = PITCH) : Front(Vec3f{ 0.0f, 0.0f, -1.0f }), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(Vec3f{ 0.0f, 0.0f, -1.0f }), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = Vec3f{ posX, posY, posZ };
        WorldUp = Vec3f{ upX, upY, upZ };
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Mat44f GetViewMatrix()
    {
        Vec3f f = normalize(Front);
        Vec3f u = normalize(Up);
        Vec3f s = normalize(cross(f, u));
        u = cross(s, f);
        return Mat44f{ {
                s.x, s.y, s.z, -dot(s, Position),
                u.x, u.y, u.z, -dot(u, Position),
                -f.x, -f.y, -f.z, dot(f, Position),
                0, 0, 0, 1} };

    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f; 
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        Vec3f front;
        front.x = cos(Yaw * PI / 180) * cos(Pitch * PI / 180);
        front.y = sin(Pitch * PI / 180);
        front.z = sin(Yaw * PI / 180) * cos(Pitch * PI / 180);
        Front = normalize(front);
        // also re-calculate the Right and Up vector
        Right = normalize(cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = normalize(cross(Right, Front));
    }
};
#endif

