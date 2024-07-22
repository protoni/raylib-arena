#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <string>
#include <vector>
#include "raylib.h"
#include "settings.h"

namespace arena {

struct AnimationInfo {
    ModelAnimation* animation;
    const char* name;
};

class AnimationManager {
   public:
    AnimationManager(const PlayerSettings& settings);
    virtual ~AnimationManager();
    bool LoadAnimations(const char* filename);
    void BlendToAnimation(const char* name, float blendTime);
    void UpdateAnimation(Model& model, float deltaTime);
    void Cleanup();
    void SetAnimationByName(const std::string& animation);
    ModelAnimation* GetCurrentAnimation() const;
    int GetCurrentFrame() const;
    const std::string& GetCurrentAnimationName() const;
    void ApplyAnimationPose(const Model& model);

   private:
    int findAnimationByName(const char* name) const;
    float getAnimationSpeedByName(const std::string& name) const;
    Transform transformLerp(const Transform& t1, const Transform& t2,
                            float amount);
    PlayerSettings m_settings;
    std::vector<AnimationInfo> m_animations;
    std::vector<float> m_animFrameCounters;
    std::map<std::string, const float> m_animationSpeedValues;
    ModelAnimation* m_animationData;
    std::string m_currentAnimName = "idle";
    float m_animFrameCounter = 0.0f;
    int m_currentAnimIndex = 0;
    int m_prevAnimIndex = 0;
    float m_blendTime = 0.0f;
    float m_blendTimer = 0.0f;
    float m_animationChangeCooldown = 0.001f;  // Adjust this value as needed
    float m_lastAnimationChangeTime = 0.0f;
    std::vector<Transform> m_boneTransforms;
};

}  // namespace arena

#endif  // ANIMATION_H