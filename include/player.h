#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "raylib.h"
#include "raymath.h"

namespace arena {

class Player {
   public:
    Player(const char* model);
    virtual ~Player();
    bool LoadPlayerModel(const char* modelPath);
    void Update(float deltaTime, const std::vector<Vector3>& colliders);
    void Draw();
    bool Initialize();

    Vector3 position;
    Vector3 velocity;
    Vector3 facing;
    float rotation;
    bool isJumping;
    bool isGrounded;

   private:
    Model m_model;
    const char* m_modelPath;
    const float JUMP_SPEED = 5.0f;
    const float MOVE_SPEED = 2.0f;
    const float PLAYER_RADIUS = 0.5f;
    const float PLAYER_HEIGHT = 1.0f;
    // Add other necessary constants and private methods
};
}  // namespace arena
#endif  // PLAYER_H