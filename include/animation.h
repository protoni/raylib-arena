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
    void UpdateAnimation(Model& model, float deltaTime);
    void Cleanup();
    int FindAnimationByName(const char* name) const;
    void SetAnimationByName(const std::string& animation);
    float GetAnimationSpeedByName(const std::string& name) const;

   private:
    PlayerSettings m_settings;
    std::vector<AnimationInfo> m_animations;
    std::map<std::string, const float> m_animationSpeedValues;
    ModelAnimation* m_animationData;
    std::string m_currentAnimName = "idle";
    float m_animFrameCounter = 0.0f;
};

}  // namespace arena

#endif  // ANIMATION_H