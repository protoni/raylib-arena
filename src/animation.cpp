#include "animation.h"
#include <iostream>
#include "logger.h"
#include "raymath.h"

namespace arena {

AnimationManager::AnimationManager(const PlayerSettings& settings)
    : m_settings(settings),
      m_currentAnimIndex(-1),
      m_prevAnimIndex(-1),
      m_blendTime(0.2f),
      m_blendTimer(0.0f) {}

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
        LOG_DEBUG("Loaded ", animCount, " animations from ", filename);
        for (int i = 0; i < animCount; i++) {
            m_animations.push_back(
                {&m_animationData[i], m_animationData[i].name});
            m_animationSpeedValues.insert(
                std::make_pair(std::string(m_animationData[i].name),
                               m_settings.defaultAnimationSpeed));
            m_animFrameCounters.push_back(0);
            LOG_DEBUG("Loaded animation: ", m_animationData[i].name,
                      " (Frames: ", m_animationData[i].frameCount,
                      ", Bones: ", m_animationData[i].boneCount, ")");
        }
    }
    return true;
}

void AnimationManager::BlendToAnimation(const char* name, float blendTime) {
    float currentTime = GetTime();
    if (currentTime - m_lastAnimationChangeTime < m_animationChangeCooldown) {
        return;
    }

    int newAnimIndex = findAnimationByName(name);
    LOG_DEBUG("Attempting to blend to animation: ", name,
              " (Index: ", newAnimIndex, ")");

    if (newAnimIndex != -1 && newAnimIndex != m_currentAnimIndex) {
        m_blendTime = blendTime;
        m_blendTimer = 0.0f;
        m_prevAnimIndex = m_currentAnimIndex;
        m_currentAnimIndex = newAnimIndex;
        m_currentAnimName = name;
        m_lastAnimationChangeTime = currentTime;
        LOG_DEBUG("Blending to new animation: ", name,
                  " (Index: ", newAnimIndex, ")");
    } else if (newAnimIndex == -1) {
        LOG_ERROR("Animation not found: ", name);
    }
}

float AnimationManager::getAnimationSpeedByName(const std::string& name) const {
    auto it = m_animationSpeedValues.find(name);
    if (it != m_animationSpeedValues.end()) {
        return it->second;
    }
    return m_settings.defaultAnimationSpeed;
}

void AnimationManager::UpdateAnimation(Model& model, float deltaTime) {
    if (m_currentAnimIndex == -1) {
        LOG_ERROR("No current animation set");
        return;
    }

    ModelAnimation* currAnim = m_animations[m_currentAnimIndex].animation;
    float currentAnimSpeed = getAnimationSpeedByName(m_currentAnimName);

    // Update animation timing
    m_animFrameCounters[m_currentAnimIndex] += deltaTime * currentAnimSpeed;
    if (m_animFrameCounters[m_currentAnimIndex] >= currAnim->frameCount)
        m_animFrameCounters[m_currentAnimIndex] = 0;

    // Get the current frame number
    int frameA = (int)m_animFrameCounters[m_currentAnimIndex];
    int frameB = (frameA + 1) % currAnim->frameCount;

    // Calculate the interpolation factor between frames
    float t = m_animFrameCounters[m_currentAnimIndex] - frameA;

    // Resize m_boneTransforms if necessary
    if (m_boneTransforms.size() != model.boneCount) {
        m_boneTransforms.resize(model.boneCount);
    }

    // Interpolate between frames and store the results
    for (int i = 0; i < model.boneCount && i < currAnim->boneCount; i++) {
        Transform transformA = currAnim->framePoses[frameA][i];
        Transform transformB = currAnim->framePoses[frameB][i];

        m_boneTransforms[i].translation =
            Vector3Lerp(transformA.translation, transformB.translation, t);
        m_boneTransforms[i].rotation =
            QuaternionSlerp(transformA.rotation, transformB.rotation, t);
        m_boneTransforms[i].scale =
            Vector3Lerp(transformA.scale, transformB.scale, t);
    }

    LOG_DEBUG("Updated animation: ", m_currentAnimName, " (Frame: ", frameA,
              " to ", frameB, ", t: ", t, ", Bone Count: ", currAnim->boneCount,
              ", Model Bone Count: ", model.boneCount, ")");

    // Log the first bone's transform
    if (!m_boneTransforms.empty()) {
        Transform& t = m_boneTransforms[0];
        LOG_DEBUG("Bone 0 transform - Translation: (", t.translation.x, ", ",
                  t.translation.y, ", ", t.translation.z, "), Rotation: (",
                  t.rotation.x, ", ", t.rotation.y, ", ", t.rotation.z, ", ",
                  t.rotation.w, "), Scale: (", t.scale.x, ", ", t.scale.y, ", ",
                  t.scale.z, ")");
    }

    // Log more details about the first bone
    if (!m_boneTransforms.empty()) {
        Transform& t = m_boneTransforms[0];
        LOG_DEBUG("Calculated Bone 0 transform - Translation: (",
                  t.translation.x, ", ", t.translation.y, ", ", t.translation.z,
                  "), Rotation: (", t.rotation.x, ", ", t.rotation.y, ", ",
                  t.rotation.z, ", ", t.rotation.w, "), Scale: (", t.scale.x,
                  ", ", t.scale.y, ", ", t.scale.z, ")");
    }

    // Log the actual model's bindPose for the first bone
    if (model.boneCount > 0) {
        Transform& t = model.bindPose[0];
        LOG_DEBUG("Model Bone 0 bindPose - Translation: (", t.translation.x,
                  ", ", t.translation.y, ", ", t.translation.z,
                  "), Rotation: (", t.rotation.x, ", ", t.rotation.y, ", ",
                  t.rotation.z, ", ", t.rotation.w, "), Scale: (", t.scale.x,
                  ", ", t.scale.y, ", ", t.scale.z, ")");
    }

    for (int i = 0; i < model.boneCount && i < currAnim->boneCount; i++) {
        model.bindPose[i] = m_boneTransforms[i];
    }

    UpdateModelAnimation(model, *currAnim,
                         (int)m_animFrameCounters[m_currentAnimIndex]);
}

void AnimationManager::ApplyAnimationPose(const Model& model) {
    if (m_boneTransforms.empty())
        return;

    for (int i = 0; i < model.boneCount && i < m_boneTransforms.size(); i++) {
        // Directly assign the calculated transform
        model.bindPose[i] = m_boneTransforms[i];
    }
}

void AnimationManager::Cleanup() {
    for (auto& anim : m_animations) {
        UnloadModelAnimation(*anim.animation);
    }
    m_animations.clear();
    m_animFrameCounters.clear();
    RL_FREE(m_animationData);
}

int AnimationManager::findAnimationByName(const char* name) const {
    for (size_t i = 0; i < m_animations.size(); i++) {
        if (strcmp(m_animations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Animation not found
}

Transform AnimationManager::transformLerp(const Transform& t1,
                                          const Transform& t2, float amount) {
    Transform result;
    result.translation = Vector3Lerp(t1.translation, t2.translation, amount);
    result.rotation = QuaternionSlerp(t1.rotation, t2.rotation, amount);
    result.scale = Vector3Lerp(t1.scale, t2.scale, amount);
    return result;
}

ModelAnimation* AnimationManager::GetCurrentAnimation() const {
    if (m_currentAnimIndex != -1) {
        return m_animations[m_currentAnimIndex].animation;
    }
    return nullptr;
}

int AnimationManager::GetCurrentFrame() const {
    if (m_currentAnimIndex != -1) {
        return (int)m_animFrameCounters[m_currentAnimIndex];
    }
    return 0;
}

const std::string& AnimationManager::GetCurrentAnimationName() const {
    return m_currentAnimName;
}

}  // namespace arena