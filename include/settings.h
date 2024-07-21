#ifndef SETTINGS_H
#define SETTINGS_H

#include "raylib.h"
#include "raymath.h"

namespace arena {

struct CameraSettings {
    float cameraDistance = 5.0f;
    float mouseSensitivity = 150.0f;
};

struct PlayerSettings {
    const char* model = "../assets/models/orc.glb";
};

struct TerrainSettings {
    const char* model = "../assets/models/map.glb";
};

struct Settings {

    CameraSettings cameraSettings;
    PlayerSettings playerSettings;
    TerrainSettings terrainSettings;
};

}  // namespace arena
#endif  // SETTINGS_H