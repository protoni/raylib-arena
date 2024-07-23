#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include "animation.h"
#include "raylib.h"
#include "raymath.h"
#include "settings.h"
#include "terrain.h"

namespace arena {

struct PlayerState {
    Vector3 position;
    Vector3 velocity = {0};
    Vector3 facingDirection;
    Vector3 movement = {0};
    float rotationHorizontal;
    float radius;
    float height;
    float moveSpeed;
    float jumpSpeed;
    bool isGrounded = false;
    bool isJumping = false;
    float groundHeight = 0;
    int lastCollidingTriangleIndex = -1;
    int collidingTriangleIndex = -1;
    float timeSinceGrounded = 0.0f;
    std::string animationState = "idle";

    PlayerState(const Vector3& pos, const Vector3& facing, const float rotation,
                const float rad, const float inHeight, const float inMoveSpeed,
                const float inJumpSpeed)
        : position(pos),
          facingDirection(facing),
          rotationHorizontal(rotation),
          radius(rad),
          height(inHeight),
          moveSpeed(inMoveSpeed),
          jumpSpeed(inJumpSpeed) {}
};

class Player {
   public:
    Player(const Settings& settings, Terrain* terrain);
    virtual ~Player();
    bool Initialize();
    void Update(float deltaTime, const std::vector<Vector3>& colliders);
    void Draw() const;
    void DrawColliders() const;
    void DrawCollisionBox() const;
    void DrawGroundHeightIndicator() const;
    const PlayerState& GetState() const { return m_state; }

   private:
    bool LoadPlayerModel(const char* modelPath);
    void checkCollisions(Vector3& newPosition);
    void updateAnimations(const Vector3& direction, const float deltaTime);
    void calculateFacingDirection(const float delta);
    Vector3 moveToFacingDirection(const float delta,
                                  const Vector3& moveDirection);
    void updateVelocity(const float delta, const Vector3& relativeMove);
    void crossOverTerrainEdges();

    Model m_model;
    PlayerState m_state;
    std::unique_ptr<AnimationManager> m_animManager;
    Settings m_appSettings;
    PlayerSettings m_settings;
    Terrain* m_terrain;

    float m_groundedTimer = 0.0f;
    const float GROUNDED_THRESHOLD = 0.1f;       // Adjust as needed
    const float JUMP_VELOCITY_THRESHOLD = 2.0f;  // Adjust as needed
    Vector3 m_terrainNormal = {0, 1, 0};
};

}  // namespace arena

#endif  // PLAYER_H