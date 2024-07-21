#include "debug.h"
#include "utils.h"
#include "logger.h"
#include <vector>

namespace arena {
namespace debug {

#define MAX_MATERIAL_MAPS 100

void PrintMaterialInfo(const Model& model) {
    for (int i = 0; i < model.materialCount; i++) {
        Material material = model.materials[i];
        LOG_DEBUG("Material: ", i, ":");
        LOG_DEBUG("  shader: ", material.shader.id);
        LOG_DEBUG("  maps: ");
        for (int j = 0; j < MAX_MATERIAL_MAPS; j++) {
            if (material.maps[j].texture.id > 0) {
                LOG_DEBUG("    Map: ", j,
                          ": Texture ID:", material.maps[j].texture.id);
            } else
                break;
        }
    }
}

void PrintColliderInfo(const Model& model, const int maxColliderCount) {
    std::vector<Vector3> colliders =
        utils::LoadCollidersFromMesh(model.meshes[0]);
    if (!colliders.empty()) {
        LOG_DEBUG("Colliders size: ", colliders.size());
        for (size_t i = 0; i < colliders.size() && i < maxColliderCount;
             ++i) {

            LOG_DEBUG("Collider ", i, ":  (", colliders[i].x, ", ",
                      colliders[i].y, ", ", colliders[i].z, ")");
        }
    }
}

}  // namespace debug
}  // namespace arena