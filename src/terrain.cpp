#include "terrain.h"
#include "utils.h"
#include "logger.h"
#include "debug.h"


namespace arena {

Terrain::Terrain(const TerrainSettings& settings) : m_modelPath(settings.model) {}

Terrain::~Terrain() {
    UnloadModel(m_model);
}

bool Terrain::Initialize() {
    if (!LoadTerrainModel(m_modelPath))
        return false;

    m_colliders = utils::LoadCollidersFromMesh(m_model.meshes[0]);

    return true;
}

bool Terrain::LoadTerrainModel(const char* modelPath) {
    m_model = LoadModel(modelPath);
    if (m_model.meshCount == 0) {
        LOG_ERROR("Failed to load terrain model.");
        return false;
    }

    LOG_INFO("Successfully loaded terrain model");
    debug::PrintMaterialInfo(m_model);

    return true;
}

void Terrain::Draw() {
    // Draw terrain model and debug colliders
    DrawModelEx(m_model, Vector3Zero(), Vector3{1, 0, 0}, 0.0f,
                Vector3One(), WHITE);
}

void Terrain::DrawCollidingTriangle(const int triangleIndex,
                                    const Vector3& colliderPosition) {
    // Draw colliding triangle
    if (triangleIndex != -1 && triangleIndex * 3 + 2 < m_colliders.size()) {
        Vector3 v1 = m_colliders[triangleIndex * 3];
        Vector3 v2 = m_colliders[triangleIndex * 3 + 1];
        Vector3 v3 = m_colliders[triangleIndex * 3 + 2];
        DrawTriangle3D(v1, v2, v3, RED);
        DrawSphere(colliderPosition, 0.1f, GRAY);
    }
}

void Terrain::DrawColliderFaces() const {
    for (size_t i = 0; i < m_colliders.size(); i += 3) {
        DrawTriangle3D(m_colliders[i], m_colliders[i + 1], m_colliders[i + 2], RED);
    }
}

void Terrain::DrawColliderEdges() const {
    for (size_t i = 0; i < m_colliders.size(); i += 3) {
        DrawLine3D(m_colliders[i], m_colliders[i + 1], RED);
        DrawLine3D(m_colliders[i + 1], m_colliders[i + 2], RED);
        DrawLine3D(m_colliders[i + 2], m_colliders[i], RED);
    }
}

std::pair<float, int> Terrain::CheckCollision(const Vector3& position,
                                              const float radius,
                                              const float height,
                                              int& outLastCollidingTriangleIndex) {
    if (m_colliders.size() % 3 != 0) {
        LOG_DEBUG("Collider vector size is not a multiple of 3. Size: ", m_colliders.size());
        return std::make_pair(-FLT_MAX, -1);
    }

    float highestPoint = -FLT_MAX;
    int collidingTriangleIndex = -1;
    float lowestGroundHeight = FLT_MAX;

    auto checkTriangle = [&](int index) {
        Vector3 v1 = m_colliders[index];
        Vector3 v2 = m_colliders[index + 1];
        Vector3 v3 = m_colliders[index + 2];

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
            if (feetHeight <= triangleHeight + m_settings.collisionHysteresis &&
                position.y + height / 2 >=
                    triangleHeight - m_settings.collisionHysteresis &&
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
    if (outLastCollidingTriangleIndex != -1 &&
        outLastCollidingTriangleIndex * 3 + 2 < m_colliders.size()) {
        if (checkTriangle(outLastCollidingTriangleIndex * 3)) {
            return std::make_pair(highestPoint, collidingTriangleIndex);
        }
    }

    // If not colliding with the last triangle, check all others
    for (size_t i = 0; i < m_colliders.size(); i += 3) {
        checkTriangle(i);
    }

    if (collidingTriangleIndex == -1) {
        return std::make_pair(lowestGroundHeight, -1);
    }

    return std::make_pair(highestPoint, collidingTriangleIndex);
}

}  // namespace arena