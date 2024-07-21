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
std::vector<Vector3> LoadCollidersFromMesh(const Mesh& mesh);

// Function to check collision between a point and a triangle
static bool CheckCollisionPointTriangle(const Vector3& point, const Vector3& v1,
                                 const Vector3& v2, const Vector3& v3);

// Function to check collision between a sphere and a triange.
// Return collision height in 'collisionHeight' ref
static bool CheckCollisionSphereTriangle(const Vector3& center,
                                         const float radius, const Vector3& v1,
                                         const Vector3& v2, const Vector3& v3,
                                         float& collisionHeight);

}  // namespace utils
}  // namespace arena
#endif  // UTILS_H