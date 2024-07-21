#include "utils.h"
#include "logger.h"
#include <vector>

namespace arena {
namespace utils {

static float Clamp(float value, float min, float max) {
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

static Vector3 BarycentricCoordinates(const Vector3& p, const Vector3& a,
                                      const Vector3& b, const Vector3& c) {
    Vector3 v0 = Vector3Subtract(b, a), v1 = Vector3Subtract(c, a),
            v2 = Vector3Subtract(p, a);
    float d00 = Vector3DotProduct(v0, v0);
    float d01 = Vector3DotProduct(v0, v1);
    float d11 = Vector3DotProduct(v1, v1);
    float d20 = Vector3DotProduct(v2, v0);
    float d21 = Vector3DotProduct(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    return Vector3{u, v, w};
}

static Vector3 Vector3ClosestPointOnTriangle(const Vector3& point,
                                             const Vector3& v1,
                                             const Vector3& v2,
                                             const Vector3& v3) {
    Vector3 edge0 = Vector3Subtract(v2, v1);
    Vector3 edge1 = Vector3Subtract(v3, v1);
    Vector3 v0 = Vector3Subtract(v1, point);

    float a = Vector3DotProduct(edge0, edge0);
    float b = Vector3DotProduct(edge0, edge1);
    float c = Vector3DotProduct(edge1, edge1);
    float d = Vector3DotProduct(edge0, v0);
    float e = Vector3DotProduct(edge1, v0);

    float det = a * c - b * b;
    float s = b * e - c * d;
    float t = b * d - a * e;

    if (s + t < det) {
        if (s < 0.f) {
            if (t < 0.f) {
                if (d < 0.f) {
                    s = Clamp(-d / a, 0.f, 1.f);
                    t = 0.f;
                } else {
                    s = 0.f;
                    t = Clamp(-e / c, 0.f, 1.f);
                }
            } else {
                s = 0.f;
                t = Clamp(-e / c, 0.f, 1.f);
            }
        } else if (t < 0.f) {
            s = Clamp(-d / a, 0.f, 1.f);
            t = 0.f;
        } else {
            float invDet = 1.f / det;
            s *= invDet;
            t *= invDet;
        }
    } else {
        if (s < 0.f) {
            float tmp0 = b + d;
            float tmp1 = c + e;
            if (tmp1 > tmp0) {
                float numer = tmp1 - tmp0;
                float denom = a - 2 * b + c;
                s = Clamp(numer / denom, 0.f, 1.f);
                t = 1 - s;
            } else {
                t = Clamp(-e / c, 0.f, 1.f);
                s = 0.f;
            }
        } else if (t < 0.f) {
            if (a + d > b + e) {
                float numer = c + e - b - d;
                float denom = a - 2 * b + c;
                s = Clamp(numer / denom, 0.f, 1.f);
                t = 1 - s;
            } else {
                s = Clamp(-e / c, 0.f, 1.f);
                t = 0.f;
            }
        } else {
            float numer = c + e - b - d;
            float denom = a - 2 * b + c;
            s = Clamp(numer / denom, 0.f, 1.f);
            t = 1.f - s;
        }
    }

    return Vector3Add(
        v1, Vector3Add(Vector3Scale(edge0, s), Vector3Scale(edge1, t)));
}

static std::vector<Vector3> LoadCollidersFromMesh(const Mesh& mesh) {
    std::vector<Vector3> colliders;

    if (mesh.vertices != nullptr && mesh.vertexCount > 0 &&
        mesh.indices != nullptr) {
        LOG_TRACE("Vertex count: ", mesh.vertexCount);
        for (int i = 0; i < mesh.triangleCount * 3; i += 3) {
            int idx1 = mesh.indices[i];
            int idx2 = mesh.indices[i + 1];
            int idx3 = mesh.indices[i + 2];
            colliders.push_back(Vector3{mesh.vertices[idx1 * 3],
                                        mesh.vertices[idx1 * 3 + 1],
                                        mesh.vertices[idx1 * 3 + 2]});
            colliders.push_back(Vector3{mesh.vertices[idx2 * 3],
                                        mesh.vertices[idx2 * 3 + 1],
                                        mesh.vertices[idx2 * 3 + 2]});
            colliders.push_back(Vector3{mesh.vertices[idx3 * 3],
                                        mesh.vertices[idx3 * 3 + 1],
                                        mesh.vertices[idx3 * 3 + 2]});
        }
    } else {
        LOG_TRACE(
            "Mesh has no vertices, vertex count is zero, or indices are "
            "missing");
    }

    return colliders;
}

}  // namespace utils
}  // namespace arena

