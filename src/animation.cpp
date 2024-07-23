#include "animation.h"
#include "logger.h"

namespace arena {

AnimationManager::AnimationManager(const PlayerSettings& settings)
    : m_settings(settings), m_currentAnimIndex(0), m_animFrameCounter(0.0f) {}

AnimationManager::~AnimationManager() {
    Cleanup();
}

bool AnimationManager::LoadAnimations(const char* filename) {
    int animCount = 0;
    ModelAnimation* animationData = LoadModelAnimations(filename, &animCount);
    if (animationData == nullptr || animCount == 0) {
        LOG_ERROR("Failed to load animations for player model.");
        return false;
    }

    for (int i = 0; i < animCount; i++) {
        m_animations.push_back(animationData[i]);
        m_animationMap[animationData[i].name] = i;
        m_animationSpeedValues[animationData[i].name] =
            m_settings.defaultAnimationSpeed;
        LOG_DEBUG("Loaded animation: ", animationData[i].name);
    }

    return true;
}

void AnimationManager::UpdateAnimation(Model& model, float deltaTime,
                                       const std::string& state) {
    auto it = m_animationMap.find(state);
    if (it == m_animationMap.end()) {
        LOG_ERROR("Animation not found: ", state);
        return;
    }

    int newAnimIndex = it->second;
    if (newAnimIndex != m_currentAnimIndex) {
        m_currentAnimIndex = newAnimIndex;
        m_animFrameCounter = 0.0f;
    }

    ModelAnimation& currentAnim = m_animations[m_currentAnimIndex];
    float currentAnimSpeed = m_animationSpeedValues[state];

    m_animFrameCounter += deltaTime * currentAnimSpeed;
    if (m_animFrameCounter >= currentAnim.frameCount) {
        m_animFrameCounter = fmodf(m_animFrameCounter, currentAnim.frameCount);
    }

    UpdateModelAnimation(model, currentAnim, (int)m_animFrameCounter);
}

void AnimationManager::Cleanup() {
    for (auto& anim : m_animations) {
        UnloadModelAnimation(anim);
    }
    m_animations.clear();
    m_animationMap.clear();
    m_animationSpeedValues.clear();
}

}  // namespace arena