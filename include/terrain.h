#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "raylib.h"

namespace arena {

static const float MAP_WIDTH = 100.0f;
static const float MAP_DEPTH = 100.0f;
static const float COLLISION_HYSTERESIS = 0.05f;

class Terrain {
   public:
    Terrain(const char* modelPath);
    bool LoadTerrainModel(const char* modelPath);
    void Draw();
    std::pair<float, int> CheckCollision(Vector3 position, float radius,
                                         float height);
    bool Initialize();

   private:
    Model terrainModel;
    const char* m_modelPath;
    std::vector<Vector3> colliders;
    int m_lastCollidingTriangleIndex = -1;
};
}  // namespace arena
#endif  // TERRAIN_H