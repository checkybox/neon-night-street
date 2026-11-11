#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <vector>

struct AABB {
    float minX, maxX, minZ, maxZ;
};

struct CircleObstacle {
    float x, z, radius;
};

Camera::Camera(glm::vec3 position)
    : Position(position),
      Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      Yaw(YAW),
      Pitch(PITCH),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY) {
    updateCameraVectors();
    clampToBounds();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

    // keep player motion on ground plane
    glm::vec3 walkFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
    glm::vec3 walkRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));

    if (direction == FORWARD)
        Position += walkFront * velocity;
    if (direction == BACKWARD)
        Position -= walkFront * velocity;
    if (direction == LEFT)
        Position -= walkRight * velocity;
    if (direction == RIGHT)
        Position += walkRight * velocity;

    clampToBounds();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}

void Camera::clampToBounds() {
    Position.y = PLAYER_HEIGHT;

    // hard perimeter boundaries
    Position.x = std::max(-18.0f, std::min(18.0f, Position.x));
    Position.z = std::max(-37.5f, std::min(17.5f, Position.z));

    const float r = 0.35f;

    // static building, dumpster and wall obstacle bounds
    static const std::vector<AABB> boxes = {
        // northwest building and perimeter
        { -25.0f, -6.2f, -40.0f, -13.0f },
        // northeast building and perimeter
        { 6.2f, 25.0f, -40.0f, -13.0f },
        // southeast commercial block
        { 6.2f, 25.0f, -2.5f, 18.0f },
        // southwest konbini main body and inset columns
        { -25.0f, -7.0f, -2.5f, 8.5f },
        { -7.0f, -6.3f, -2.5f, -0.4f },
        { -7.0f, -6.3f, 6.4f, 8.5f },
        // parking lot rear wall and south divider
        { -14.0f, -11.5f, 8.5f, 18.0f },
        { -14.0f, -4.0f, 17.5f, 20.0f },
        // west alley dumpster, back wall and cardboard crate
        { -19.5f, -18.0f, -13.0f, -3.0f },
        { -18.2f, -16.2f, -9.5f, -6.5f },
        { -17.8f, -16.6f, -6.6f, -5.4f },
        // vending machines against alley wall
        { -10.4f, -7.5f, -3.7f, -2.5f }
    };

    for (const auto& b : boxes) {
        if (Position.x > b.minX - r && Position.x < b.maxX + r &&
            Position.z > b.minZ - r && Position.z < b.maxZ + r) {
            float dLeft   = Position.x - (b.minX - r);
            float dRight  = (b.maxX + r) - Position.x;
            float dBottom = Position.z - (b.minZ - r);
            float dTop    = (b.maxZ + r) - Position.z;

            float minD = std::min(std::min(dLeft, dRight), std::min(dBottom, dTop));
            if (minD == dLeft)   Position.x = b.minX - r;
            else if (minD == dRight)  Position.x = b.maxX + r;
            else if (minD == dBottom) Position.z = b.minZ - r;
            else Position.z = b.maxZ + r;
        }
    }

    // cylindrical obstacles: streetlamp poles and trash cans
    static const std::vector<CircleObstacle> circles = {
        { -4.2f, -30.0f, 0.40f },
        { -4.2f, -18.0f, 0.40f },
        { -4.2f,  -1.0f, 0.40f },
        { -4.2f,   7.0f, 0.40f },
        {  4.2f, -30.0f, 0.40f },
        {  4.2f, -18.0f, 0.40f },
        {  4.2f,  -1.0f, 0.40f },
        {  4.2f,  13.0f, 0.40f },
        { -10.8f, -3.1f, 0.35f }
    };

    for (const auto& c : circles) {
        float dx = Position.x - c.x;
        float dz = Position.z - c.z;
        float distSq = dx * dx + dz * dz;
        float minR = c.radius + r;
        if (distSq < minR * minR) {
            float dist = std::sqrt(distSq);
            if (dist > 0.0001f) {
                float push = minR - dist;
                Position.x += (dx / dist) * push;
                Position.z += (dz / dist) * push;
            }
        }
    }
}
