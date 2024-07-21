#ifndef PLAYER_H
#define PLAYER_H

#include "animation.h"
#include "settings.h"
#include "terrain.h"

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

namespace arena {

struct State {

    Vector3 position;
    Vector3 velocity = {0};
    Vector3 facingDirection;
    float rotationHorizontal;
    float radius;
    float height;
    float moveSpeed;
    float jumpSpeed;
    bool isGrounded = false;
    bool isJumping = false;

    State(
        const Vector3& pos,
        const Vector3& facing,
        const float rotation,
        const float rad,
        const float inHeight,
        const float inMoveSpeed,
        const float inJumpSpeed)
        : position(pos)
        , facingDirection(facing)
        , rotationHorizontal(rotation)
        , radius(rad)
        , height(inHeight)
        , moveSpeed(inMoveSpeed)
        , jumpSpeed(inJumpSpeed) {}
};

class Player {
   public:
    Player(const Settings& settings, Terrain* terrain);
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
    void checkCollisions(Vector3& newPosition);
    void updateAnimations(const Vector3& direction) const;
    void calculateFacingDirection(const float delta);

    // Apply movement vector to direction. Returns the relative move.
    Vector3 moveToFacingDirection(const float delta,
                                  const Vector3& moveDirection);

    void updateVelocity(const float delta, const Vector3& relativeMove);

    Model m_model;
    State m_state;
    std::unique_ptr<AnimationManager> m_animManager;
    Settings m_appSettings;
    PlayerSettings m_settings;
    Vector3 m_playerPosition;
    Vector3 m_playerVelocity;
    const float JUMP_SPEED = 5.0f;
    const float MOVE_SPEED = 2.0f;
    const float PLAYER_RADIUS = 0.5f;
    const float PLAYER_HEIGHT = 1.0f;
    Terrain* m_terrain;
    
};
}  // namespace arena
#endif  // PLAYER_H