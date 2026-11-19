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

static float getGroundHeight(float x, float z) {
    // driveway ramp into konbini parking bay
    if (x <= -4.0f && x >= -6.2f && z >= 8.5f && z <= 13.5f) {
        return 0.03f;
    }

    // main avenue road carriageway
    bool inAvenueRoad = (x > -4.0f && x < 4.0f);

    // crossroad east-west street carriageway
    bool inCrossroadRoad = (z > -11.5f && z < -4.5f);

    if (inAvenueRoad || inCrossroadRoad) {
        return 0.0f;
    }

    // sidewalks, alleys, parking bays and shop entrances
    return 0.20f;
}

Camera::Camera(glm::vec3 position)
    : Position(position),
      Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      Yaw(YAW),
      Pitch(PITCH),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY) {
    Position.y = PLAYER_HEIGHT + getGroundHeight(Position.x, Position.z);
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

void Camera::update(float deltaTime) {
    float groundY = getGroundHeight(Position.x, Position.z);
    float targetY = PLAYER_HEIGHT + groundY;

    if (deltaTime > 0.0001f) {
        float diff = targetY - Position.y;
        if (std::abs(diff) < 0.002f) {
            Position.y = targetY;
        } else {
            Position.y += diff * std::min(1.0f, deltaTime * 14.0f);
        }
    } else {
        Position.y = targetY;
    }
}

void Camera::clampToBounds() {
    // hard perimeter boundaries
    Position.x = std::max(-18.0f, std::min(18.0f, Position.x));
    Position.z = std::max(-73.0f, std::min(43.0f, Position.z));

    const float r = 0.35f;

    // static building, dumpster and wall obstacle bounds
    static const std::vector<AABB> boxes = {
        // north avenue east corridor with walkable alley 1
        {  6.2f, 25.0f, -75.0f, -46.0f },
        { 13.5f, 25.0f, -46.0f, -43.5f },
        {  6.2f, 25.0f, -43.5f, -13.0f },
        // north avenue west corridor with walkable alley 2 and parking alcove
        { -25.0f, -6.2f, -75.0f, -46.0f },
        { -25.0f, -13.5f, -46.0f, -43.5f },
        { -25.0f, -6.2f, -43.5f, -36.0f },
        { -25.0f, -6.2f, -36.0f, -30.0f },
        { -25.0f, -6.2f, -30.0f, -13.0f },
        // south avenue east corridor with walkable alley 3
        {  6.2f, 25.0f, -2.5f, 8.5f },
        { 13.5f, 25.0f,  8.5f, 11.2f },
        {  6.2f, 25.0f, 11.2f, 45.0f },
        // south avenue west corridor
        { -25.0f, -7.0f, -2.5f, 8.5f },
        { -7.0f, -6.3f, -2.5f, -0.4f },
        { -7.0f, -6.3f, 6.4f, 8.5f },
        { -14.0f, -11.5f, 8.5f, 18.0f },
        { -25.0f, -6.2f, 18.0f, 45.0f },
        // alley 1 trash pile and boxes
        {   7.8f,   9.6f, -45.7f, -44.8f },
        {  10.6f,  12.6f, -45.1f, -44.0f },
        // alley 2 dumpster, trash bags and boxes
        { -12.5f, -10.5f, -45.7f, -43.8f },
        {  -9.8f,  -8.3f, -45.6f, -44.1f },
        {  -8.6f,  -7.5f, -45.5f, -44.6f },
        // alley 3 trash bags and crates
        {   8.2f,  11.2f,   9.1f,   9.8f },
        // west crossroad dead-end wall, return walls, and chain-link fence
        { -20.0f, -18.0f, -14.5f,   9.5f },
        { -18.8f, -13.8f, -14.5f, -13.5f },
        { -18.8f, -12.0f,   8.2f,   8.8f },
        { -18.5f, -12.5f,  -2.8f,  -2.2f },
        // west side-alley dumpster and shipping crates
        { -17.3f, -15.1f, -13.6f, -12.0f },
        { -15.0f, -13.8f, -13.8f, -12.6f },
        // east crossroad dead-end wall and perimeter return walls
        {  18.0f,  20.0f, -14.5f,  -2.2f },
        {  17.5f,  18.2f, -12.5f,  -3.5f },
        {  13.8f,  18.5f, -14.5f, -13.5f },
        {  13.8f,  18.5f,  -2.8f,  -2.2f },
        // east dead-end stacked wooden pallets and cardboard boxes
        {  16.8f,  18.2f, -11.2f,  -9.8f },
        {  16.9f,  17.9f,  -5.0f,  -4.0f },
        // vending machines against alley wall
        { -10.4f, -7.5f, -3.7f, -2.5f },
        // facade vending machines on extended avenue
        {  5.60f,  6.45f, -49.5f, -48.5f },
        { -6.45f, -5.60f,  37.5f,  38.5f },
        // parked vehicles in konbini parking bays
        { -11.20f, -7.00f,   9.35f,  11.25f },
        { -11.20f, -7.00f,  14.55f,  16.45f },
        // curbside parallel parked vehicles
        {  -5.50f, -3.60f, -59.60f, -55.40f },
        {   3.60f,  5.50f, -40.10f, -35.90f },
        {   3.60f,  5.50f,  18.90f,  23.10f },
        {  -5.50f, -3.60f,  30.90f,  35.10f }
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
        // west avenue streetlamp poles
        { -4.2f, -64.0f, 0.40f },
        { -4.2f, -48.0f, 0.40f },
        { -4.2f, -30.0f, 0.40f },
        { -4.2f, -18.0f, 0.40f },
        { -4.2f,  -1.0f, 0.40f },
        { -4.2f,   7.0f, 0.40f },
        { -4.2f,  27.0f, 0.40f },
        { -4.2f,  39.0f, 0.40f },
        // east avenue streetlamp poles
        {  4.2f, -64.0f, 0.40f },
        {  4.2f, -48.0f, 0.40f },
        {  4.2f, -30.0f, 0.40f },
        {  4.2f, -18.0f, 0.40f },
        {  4.2f,  -1.0f, 0.40f },
        {  4.2f,  13.0f, 0.40f },
        {  4.2f,  27.0f, 0.40f },
        {  4.2f,  39.0f, 0.40f },
        // trash can in konbini alley
        { -10.8f,  -3.1f, 0.35f }
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
