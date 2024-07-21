#ifndef GAME_H
#define GAME_H

#include "animation.h"
#include "camera.h"
#include "game.h"
#include "player.h"
#include "rlights.h"
#include "terrain.h"
#include "settings.h"
#include "shader_handler.h"
#include <memory>

namespace arena {

class Game {
   public:
    bool Initialize();
    void Update();
    void Draw();
    void Cleanup();

   private:
    Settings m_settings;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<AnimationManager> m_animManager;
    std::unique_ptr<ShaderHandler> m_shaderHandler;
    Light m_lights[1] = {0};
};

}  // namespace arena

#endif  // GAME_H