#ifndef SETTINGS_H
#define SETTINGS_H

#include "raylib.h"
#include "raymath.h"
#include <string>

namespace arena {

struct CameraSettings {
    float cameraDistance = 5.0f;
    float mouseSensitivity = 150.0f;
};

struct PlayerSettings {
    const char* model = "../assets/models/orc.glb";
    const std::string initialAnimationName = "idle";
    const Vector3 initialPlayerPosition = {0.0f, 5.0f, 0.0f};
    const Vector3 initialPlayerScale = {0.5f, 0.5f, 0.5f};
    const Vector3 initialPlayerVelocity = {0, 0, 0};
    const Vector3 initialPlayerFacingDirection = {1, 0, 0};
    const float initialPlayerRotationHorizontal = 0.0f;
    const float initialPlayerRadius = 0.5f;
    const float initialPlayerHeight = 1.0f;
    const float initialMoveSpeed = 2.0f;
    const float initialJumpSpeed = 5.0f;
};

struct TerrainSettings {
    const char* model = "../assets/models/map.glb";
};

struct PhysicsSettings {
    const float gravity = -9.8f;
    const float collisionGroundCheckDistance = 0.1f;
    const float airControl = 0.3f;    // Reduced control in air
    const float airFriction = 0.99f;  // Slight friction in air
};

struct Settings {

    CameraSettings cameraSettings;
    PlayerSettings playerSettings;
    TerrainSettings terrainSettings;
    PhysicsSettings physicsSettings;
};

}  // namespace arena
#endif  // SETTINGS_H