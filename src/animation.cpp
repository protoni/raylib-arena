#include "animation.h"
#include "logger.h"
#include <iostream>


namespace arena {
AnimationManager::AnimationManager(const PlayerSettings& settings)
    : m_settings(settings) {}

AnimationManager::~AnimationManager() {
    Cleanup();
}

bool AnimationManager::LoadAnimations(const char* filename) {
    int animCount = 0;
    m_animationData = LoadModelAnimations(filename, &animCount);
    if (m_animationData == nullptr || animCount == 0) {
        LOG_ERROR("Failed to load animations for player model.");
        return false;
    } else {
        for (int i = 0; i < animCount; i++) {
            m_animations.push_back({&m_animationData[i], m_animationData[i].name});
            m_animationSpeedValues.insert(
                std::make_pair(std::string(m_animationData[i].name),
                               m_settings.defaultAnimationSpeed));
            LOG_DEBUG("Loaded animation: ", m_animationData[i].name);
        }
    }
}

void AnimationManager::SetAnimationByName(const std::string& animation) {
    m_currentAnimName = animation;
}

float AnimationManager::GetAnimationSpeedByName(const std::string& name) const {
    auto it = m_animationSpeedValues.find(name);
    if (it != m_animationSpeedValues.end()) {
        return it->second;
    }

    return m_settings.defaultAnimationSpeed;
}

void AnimationManager::UpdateAnimation(Model& model, float deltaTime) {
    // Update animation
    int currentAnimIndex =
        FindAnimationByName(m_currentAnimName.c_str());
    if (currentAnimIndex == -1) {
        LOG_ERROR("Animation not found !");
        currentAnimIndex = 0;  // Use the first animation as a fallback
    }

    m_animFrameCounter +=
        deltaTime * GetAnimationSpeedByName(m_currentAnimName);
    if (currentAnimIndex >= 0 && currentAnimIndex < m_animations.size()) {
        UpdateModelAnimation(model,
                             *m_animations[currentAnimIndex].animation,
                             m_animFrameCounter);
        if (m_animFrameCounter >=
            m_animations[currentAnimIndex].animation->frameCount) {
            m_animFrameCounter = 0;
        }
    }
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