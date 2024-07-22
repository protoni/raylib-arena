#include "player.h"
#include "debug.h"
#include "logger.h"
#include "utils.h"

namespace arena {

Player::Player(const Settings& settings, Terrain* terrain)
    : m_appSettings(settings),
      m_settings(settings.playerSettings),
      m_terrain(terrain),
      m_state(settings.playerSettings.initialPlayerPosition,
              settings.playerSettings.initialPlayerFacingDirection,
              settings.playerSettings.initialPlayerRotationHorizontal,
              settings.playerSettings.initialPlayerRadius,
              settings.playerSettings.initialPlayerHeight,
              settings.playerSettings.initialMoveSpeed,
              settings.playerSettings.initialJumpSpeed) {}

Player::~Player() {
    UnloadModel(m_model);
}

bool Player::LoadPlayerModel(const char* modelPath) {
    m_model = LoadModel(modelPath);
    if (m_model.meshCount == 0) {
        LOG_ERROR("Failed to load player model from path: ", modelPath);
        return false;
    }
    LOG_INFO("Successfully loaded player model");
    debug::PrintMaterialInfo(m_model);
    return true;
}

void Player::updateAnimations(const Vector3& direction) const {
    if (m_state.isJumping || !m_state.isGrounded) {
        m_animManager->SetAnimationByName("jump_land");
    } else if (Vector3Length(direction) > 0) {
        m_animManager->SetAnimationByName("walk");
    } else {
        m_animManager->SetAnimationByName("idle");
    }
}

void Player::calculateFacingDirection(const float delta) {
    // Calculate player facing direction
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouseDelta = GetMouseDelta();
        float rotationY = mouseDelta.x *
                          m_appSettings.cameraSettings.mouseSensitivity * delta;
        m_state.facingDirection = Vector3RotateByAxisAngle(
            m_state.facingDirection, Vector3{0, 1, 0}, -rotationY * DEG2RAD);
        m_state.facingDirection = Vector3Normalize(m_state.facingDirection);
    }
}

Vector3 Player::moveToFacingDirection(const float delta,
                                      const Vector3& moveDirection) {
    // Apply movement relative to player's facing direction
    float moveSpeed = 2.0f;
    Matrix rotationMatrix = MatrixRotateY(
        -atan2f(m_state.facingDirection.z, m_state.facingDirection.x));
    Vector3 relativeMove = Vector3Transform(moveDirection, rotationMatrix);

    // Normalize movement direction
    if (Vector3Length(moveDirection) > 0) {
        float targetRotation = atan2f(relativeMove.x, relativeMove.z);
        float rotationDiff = targetRotation - m_state.rotationHorizontal;

        // Normalize the rotation difference to [-PI, PI]
        if (rotationDiff > PI)
            rotationDiff -= 2 * PI;
        if (rotationDiff < -PI)
            rotationDiff += 2 * PI;

        // Smoothly interpolate the rotation
        m_state.rotationHorizontal += rotationDiff * 10.0f * delta;

        // Normalize the rotation to [0, 2*PI]
        while (m_state.rotationHorizontal < 0)
            m_state.rotationHorizontal += 2 * PI;
        while (m_state.rotationHorizontal >= 2 * PI)
            m_state.rotationHorizontal -= 2 * PI;
    }

    return relativeMove;
}

void Player::updateVelocity(const float delta, const Vector3& relativeMove) {
    float airControl = m_appSettings.physicsSettings.airControl;
    float airFriction = m_appSettings.physicsSettings.airFriction;

    if (m_state.isGrounded) {
        // On ground, directly set velocity based on input
        m_state.velocity.x = relativeMove.x * m_state.moveSpeed;
        m_state.velocity.z = relativeMove.z * m_state.moveSpeed;
    } else {
        // In air, apply reduced control and maintain momentum
        if (Vector3Length(relativeMove) > 0) {
            // Apply air control when there's input
            m_state.velocity.x +=
                relativeMove.x * m_state.moveSpeed * airControl * delta;
            m_state.velocity.z +=
                relativeMove.z * m_state.moveSpeed * airControl * delta;
        }

        // Apply air friction regardless of input
        m_state.velocity.x *= powf(airFriction, delta);
        m_state.velocity.z *= powf(airFriction, delta);
    }

    // Optionally, cap the horizontal velocity to prevent excessive speeds
    float maxHorizontalSpeed =
        m_state.moveSpeed * 1.5f;  // Adjust this multiplier as needed
    float horizontalSpeed = sqrtf(m_state.velocity.x * m_state.velocity.x +
                                  m_state.velocity.z * m_state.velocity.z);
    if (horizontalSpeed > maxHorizontalSpeed) {
        float scale = maxHorizontalSpeed / horizontalSpeed;
        m_state.velocity.x *= scale;
        m_state.velocity.z *= scale;
    }
}

void Player::Update(float deltaTime, const std::vector<Vector3>& colliders) {
    // Handle movement, jumping, collision, etc.
    // This will contain most of the player update logic from your main loop

    // Calculate player facing direction
    calculateFacingDirection(deltaTime);

    // Calculate movement direction
    Vector3 moveDirection = {0};
    if (IsKeyDown(KEY_W))
        moveDirection.x += 1.0f;
    if (IsKeyDown(KEY_S))
        moveDirection.x -= 1.0f;
    if (IsKeyDown(KEY_A))
        moveDirection.z -= 1.0f;
    if (IsKeyDown(KEY_D))
        moveDirection.z += 1.0f;

    // Apply direction
    Vector3 relativeMove = moveToFacingDirection(deltaTime, moveDirection);

    updateVelocity(deltaTime, relativeMove);

    // Change animations
    updateAnimations(moveDirection);

    // Apply gravity
    if (!m_state.isGrounded) {
        const float maxFallSpeed =
            -20.0f;  // Apply gravity with a maximum fall speed
        m_state.velocity.y += m_appSettings.physicsSettings.gravity * deltaTime;
        m_state.velocity.y = std::max(m_state.velocity.y, maxFallSpeed);
    }

    // Calculate new position
    Vector3 newPosition =
        Vector3Add(m_state.position, Vector3Scale(m_state.velocity, deltaTime));

    // Calculate player collisions
    checkCollisions(newPosition);

    // Handle jumping
    if (IsKeyPressed(KEY_SPACE) && m_state.isGrounded) {
        m_state.velocity.y = m_state.jumpSpeed;
        m_state.isJumping = true;
        m_state.isGrounded = false;
    }

    // Apply movement
    m_state.movement = Vector3Subtract(newPosition, m_state.position);
    if (Vector3Length(m_state.movement) > m_settings.movementThreshold) {
        m_state.position = newPosition;
    } else if (m_state.isGrounded) {
        // If grounded and movement is small, just update Y
        m_state.position.y = newPosition.y;
    }

    // Clamp player position to map boundaries
    newPosition.x =
        utils::Clamp(newPosition.x, -m_appSettings.terrainSettings.mapWidth / 2,
                     m_appSettings.terrainSettings.mapWidth / 2);
    newPosition.z =
        utils::Clamp(newPosition.z, -m_appSettings.terrainSettings.mapDepth / 2,
                     m_appSettings.terrainSettings.mapDepth / 2);

    // Calculate how much the player has moved this frame
    if (Vector3Length(m_state.movement) > m_settings.movementThreshold) {
        m_state.position = newPosition;
    } else {
        m_state.movement = Vector3Zero();
    }

    // Ignore very small movements
    if (fabs(m_state.velocity.y) < m_settings.velocityThreshold) {
        m_state.velocity.y = 0;
    }

    // Smooth out small fluctuations in y-position
    if (fabs(m_state.position.y - m_state.groundHeight - m_state.radius) <
        0.01f) {
        m_state.position.y = m_state.groundHeight + m_state.radius;
    }

    // Print player position for debugging
    LOG_DEBUG("Player position: ");
    debug::PrintVec3(m_state.position);

    // Update animations
    m_animManager->UpdateAnimation(m_model, deltaTime);
}

void Player::Draw() const {

    // Draw player model
    Vector3 modelPosition = {
        m_state.position.x,
        m_state.position.y -
            m_state.height /
                2,  // Adjust the model to sit on top of the collision box
        m_state.position.z};
    DrawModelEx(m_model, modelPosition, Vector3{0, 1, 0},
                m_state.rotationHorizontal * RAD2DEG,
                m_settings.initialPlayerScale, WHITE);

    // Highlight colliding triangle in the terrain
    m_terrain->DrawCollidingTriangle(m_state.collidingTriangleIndex,
                                     m_state.position);
}

void Player::DrawColliders() const {
    // Draw ground height indicator only when close to the ground
    if (m_state.position.y - m_state.groundHeight < m_state.height) {
        Vector3 groundPoint = {m_state.position.x, m_state.groundHeight,
                               m_state.position.z};
        DrawSphere(groundPoint, 0.1f, YELLOW);
    }
}

void Player::DrawCollisionBox() const {
    DrawCubeWires(m_state.position, m_state.radius * 2, m_state.height,
                  m_state.radius * 2, GREEN);
}

void Player::DrawGroundHeightIndicator() const {
    Vector3 groundPoint = {m_state.position.x, m_state.groundHeight,
                           m_state.position.z};
    DrawSphere(groundPoint, 0.1f, YELLOW);
}

bool Player::Initialize() {
    if (!LoadPlayerModel(m_settings.model))
        return false;

    // Load animations
    m_animManager = std::make_unique<AnimationManager>(m_settings);
    if (!m_animManager->LoadAnimations(m_settings.model)) {
        LOG_ERROR("Failed to load animations");
        return false;
    }

    return true;
}

void Player::checkCollisions(Vector3& newPosition) {
    std::pair<float, int> collisionResult =
        m_terrain->CheckCollision(newPosition, m_state.radius, m_state.height,
                                  m_state.lastCollidingTriangleIndex);
    m_state.groundHeight = collisionResult.first;
    m_state.collidingTriangleIndex = collisionResult.second;

    float feetHeight = newPosition.y - m_state.height / 2;
    float distanceToGround = feetHeight - m_state.groundHeight;

    const float maxSnapDistance = 0.1f;  // Adjust this value as needed

    if (m_state.collidingTriangleIndex != -1) {
        if (distanceToGround <= maxSnapDistance && m_state.velocity.y <= 0) {
            newPosition.y = m_state.groundHeight + m_state.height / 2;
            m_state.velocity.y = 0;
            m_state.isGrounded = true;
            m_state.isJumping = false;
        } else if (distanceToGround < 0) {
            // We're inside the ground, push the player out
            newPosition.y = m_state.groundHeight + m_state.height / 2;
            if (m_state.velocity.y < 0) {
                m_state.velocity.y = 0;
            }
            m_state.isGrounded = true;
            m_state.isJumping = false;
        } else {
            m_state.isGrounded = false;
        }
    } else {
        m_state.isGrounded = false;
    }

    m_state.lastCollidingTriangleIndex = m_state.collidingTriangleIndex;

    // Debug logging
    LOG_DEBUG("Player Y velocity: ", m_state.velocity.y);
    LOG_DEBUG("Is grounded: ", m_state.isGrounded);
    LOG_DEBUG("Is jumping: ", m_state.isJumping);
    LOG_DEBUG("Distance to ground: ", distanceToGround);
}

}  // namespace arena