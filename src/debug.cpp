#include "debug.h"
#include "logger.h"

namespace arena
{

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

}  // namespace arena