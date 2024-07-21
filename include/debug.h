#ifndef DEBUG_H
#define DEBUG_H

#include "raylib.h"

namespace arena {
namespace debug {

void PrintMaterialInfo(const Model& model);

void PrintColliderInfo(const Model& model, const int maxColliderCount = 10);

}  // namespace debug
}  // namespace arena
#endif  // DEBUG_H