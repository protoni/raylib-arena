#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <string>
#include <vector>
#include "raylib.h"
#include "settings.h"

namespace arena {

class AnimationManager {
   public:
    AnimationManager(const PlayerSettings& settings);
    virtual ~AnimationManager();
    bool LoadAnimations(const char* filename);
    void UpdateAnimation(Model& model, float deltaTime,
                         const std::string& state);
    void Cleanup();

   private:
    PlayerSettings m_settings;
    std::vector<ModelAnimation> m_animations;
    std::map<std::string, int> m_animationMap;
    std::map<std::string, float> m_animationSpeedValues;
    int m_currentAnimIndex;
    float m_animFrameCounter;
};

}  // namespace arena

#endif  // ANIMATION_H