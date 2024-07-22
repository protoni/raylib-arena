#ifndef TERRAIN_H
#define TERRAIN_H

#include "settings.h"
#include <vector>
#include "raylib.h"

namespace arena {


class Terrain {
   public:
    Terrain(const TerrainSettings& settings);
    virtual ~Terrain();
    bool LoadTerrainModel(const char* modelPath);
    void Draw();
    void DrawCollidingTriangle(const int triangleIndex,
                               const Vector3& colliderPosition);
    void DrawColliderFaces() const;
    void DrawColliderEdges() const;
    std::pair<float, int> CheckCollision(const Vector3& position, const float radius,
                                         const float height,
                                         int& outLastCollidingTriangleIndex);
    bool Initialize();
    const std::vector<Vector3>& GetColliders() const { return m_colliders; }
    

   private:
    TerrainSettings m_settings;
    Model m_model;
    const char* m_modelPath;
    std::vector<Vector3> m_colliders;
};
}  // namespace arena
#endif  // TERRAIN_H