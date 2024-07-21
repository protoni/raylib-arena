#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"
#include "raymath.h"

namespace arena {
namespace utils {

static float Clamp(float value, float min, float max);
static Vector3 BarycentricCoordinates(const Vector3& p, const Vector3& a,
                                      const Vector3& b, const Vector3& c);
static Vector3 Vector3ClosestPointOnTriangle(const Vector3& point,
                                             const Vector3& v1,
                                             const Vector3& v2,
                                             const Vector3& v3);

// Function to load colliders from a mesh
static std::vector<Vector3> LoadCollidersFromMesh(const Mesh& mesh);

}  // namespace utils
}  // namespace arena
#endif  // UTILS_H