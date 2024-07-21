#include "terrain.h"
#include "iostream"
#include "utils.h"

namespace arena {

Terrain::Terrain(const char* modelPath) : m_modelPath(modelPath) {}

bool Terrain::Initialize() {
    return LoadTerrainModel(m_modelPath);
}

bool Terrain::LoadTerrainModel(const char* modelPath) {
    Model terrainModel = LoadModel("../assets/models/map.glb");
    if (terrainModel.meshCount == 0) {
        std::cerr << "Failed to load terrain model." << std::endl;
        CloseWindow();
        return false;
    }

    return true;
}

void Terrain::Draw() {
    // Draw terrain model and debug colliders
}

std::pair<float, int> Terrain::CheckCollision(Vector3 position, float radius,
                                              float height) {
    if (colliders.size() % 3 != 0) {
        std::cerr << "Collider vector size is not a multiple of 3. Size: "
                  << colliders.size() << std::endl;
        return std::make_pair(-FLT_MAX, -1);
    }

    float highestPoint = -FLT_MAX;
    int collidingTriangleIndex = -1;
    const float COLLISION_HYSTERESIS = 0.05f;
    float lowestGroundHeight = FLT_MAX;

    auto checkTriangle = [&](int index) {
        Vector3 v1 = colliders[index];
        Vector3 v2 = colliders[index + 1];
        Vector3 v3 = colliders[index + 2];

        // Project the position onto the triangle's plane
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(
            Vector3Subtract(v2, v1), Vector3Subtract(v3, v1)));
        float d = -Vector3DotProduct(normal, v1);
        float t = -(Vector3DotProduct(normal, position) + d) /
                  Vector3DotProduct(normal, normal);
        Vector3 projection = Vector3Add(position, Vector3Scale(normal, t));

        // Check if the projection is inside the triangle using barycentric coordinates
        Vector3 barycentric = utils::BarycentricCoordinates(projection, v1, v2, v3);
        if (barycentric.x >= 0 && barycentric.y >= 0 && barycentric.z >= 0) {
            float triangleHeight = projection.y;
            float feetHeight = position.y - height / 2;
            if (feetHeight <= triangleHeight + COLLISION_HYSTERESIS &&
                position.y + height / 2 >=
                    triangleHeight - COLLISION_HYSTERESIS &&
                triangleHeight > highestPoint) {
                highestPoint = triangleHeight;
                collidingTriangleIndex = index / 3;

                if (triangleHeight < lowestGroundHeight) {
                    lowestGroundHeight = triangleHeight;
                }

                return true;
            }
        }

        return false;
    };

    // First, check the last colliding triangle
    if (m_lastCollidingTriangleIndex != -1 &&
        m_lastCollidingTriangleIndex * 3 + 2 < colliders.size()) {
        if (checkTriangle(m_lastCollidingTriangleIndex * 3)) {
            return std::make_pair(highestPoint, collidingTriangleIndex);
        }
    }

    // If not colliding with the last triangle, check all others
    for (size_t i = 0; i < colliders.size(); i += 3) {
        checkTriangle(i);
    }

    if (collidingTriangleIndex == -1) {
        return std::make_pair(lowestGroundHeight, -1);
    }

    return std::make_pair(highestPoint, collidingTriangleIndex);
}

}  // namespace arena