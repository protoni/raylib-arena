#include "animation.h"
#include <iostream>

namespace arena {

bool AnimationManager::LoadAnimations(const char* filename) {
    // Load animations from file
}

void AnimationManager::UpdateAnimation(Model& model,
                                       const std::string& animName,
                                       float& frameCounter, float deltaTime) {
    // Update current animation
}

void AnimationManager::Cleanup() {
    // Unload animations
}

int AnimationManager::FindAnimationByName(const char* name) {
    // Find animation index by name
}

}  // namespace arena