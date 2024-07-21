#include "animation.h"
#include "logger.h"
#include <iostream>

namespace arena {

bool AnimationManager::LoadAnimations(const char* filename) {
    int animCount = 0;
    m_animationData = LoadModelAnimations(filename, &animCount);
    if (m_animationData == nullptr || animCount == 0) {
        LOG_ERROR("Failed to load animations for player model.");
        return false;
    } else {
        for (int i = 0; i < animCount; i++) {
            m_animations.push_back({&m_animationData[i], m_animationData[i].name});
            LOG_DEBUG("Loaded animation: ", m_animationData[i].name);
        }
    }
}

void AnimationManager::SetAnimationByName(const std::string& animation) {
    currentAnimName = animation;
}

void AnimationManager::UpdateAnimation(Model& model,
                                       const std::string& animName,
                                       float& frameCounter, float deltaTime) {
    // Update current animation
}

void AnimationManager::Cleanup() {
    for (auto& anim : m_animations) {
        UnloadModelAnimation(*anim.animation);
    }
    m_animations.clear();
    RL_FREE(m_animationData);
}

int AnimationManager::FindAnimationByName(const char* name) const {
    for (size_t i = 0; i < m_animations.size(); i++) {
        if (strcmp(m_animations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Animation not found
}

}  // namespace arena