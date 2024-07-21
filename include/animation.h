#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <vector>
#include "raylib.h"

namespace arena {

struct AnimationInfo {
    ModelAnimation* animation;
    const char* name;
};

class AnimationManager {
   public:
    bool LoadAnimations(const char* filename);
    void UpdateAnimation(Model& model, const std::string& animName,
                         float& frameCounter, float deltaTime);
    void Cleanup();

   private:
    std::vector<AnimationInfo> animations;
    int FindAnimationByName(const char* name);
};

}  // namespace arena

#endif  // ANIMATION_H