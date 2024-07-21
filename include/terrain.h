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
    virtual ~Terrain();
    bool LoadTerrainModel(const char* modelPath);
    void Draw();
    std::pair<float, int> CheckCollision(const Vector3& position, const float radius,
                                         const float height);
    bool Initialize();
    const std::vector<Vector3>& GetColliders() const { return m_colliders; }

   private:
    Model m_model;
    const char* m_modelPath;
    std::vector<Vector3> m_colliders;
    int m_lastCollidingTriangleIndex = -1;
};
}  // namespace arena
#endif  // TERRAIN_H