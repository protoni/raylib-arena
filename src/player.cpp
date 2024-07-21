#include "player.h"
#include "utils.h"
#include "debug.h"
#include "logger.h"

namespace arena {

Player::Player(const Settings& settings, Terrain* terrain) 
    : 
    m_appSettings(settings),
    m_settings(settings.playerSettings),
    m_terrain(terrain),
    m_state(
        settings.playerSettings.initialPlayerPosition,
        settings.playerSettings.initialPlayerFacingDirection,
        settings.playerSettings.initialPlayerRotationHorizontal,
        settings.playerSettings.initialPlayerRadius,
        settings.playerSettings.initialPlayerHeight) {}


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
    // Change animations
    if (Vector3Length(direction) > 0 && !m_state.isJumping) {
        m_animManager->SetAnimationByName("walk");
    } else if (m_state.isJumping) {
        m_animManager->SetAnimationByName("jump_land");
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
    Matrix rotationMatrix =
        MatrixRotateY(-atan2f(m_state.facingDirection.z, m_state.facingDirection.x));
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
    // Update velocity based on input
    if (m_state.isGrounded) {
        m_state.velocity.x = relativeMove.x * m_state.moveSpeed;
        m_state.velocity.z = relativeMove.z * m_state.moveSpeed;
    } else {
        // In air, apply reduced control
        float airControl = m_appSettings.physicsSettings.airControl;
        float airFriction = m_appSettings.physicsSettings.airFriction;
        m_state.velocity.x +=
            relativeMove.x * m_state.moveSpeed * airControl * delta;
        m_state.velocity.z +=
            relativeMove.z * m_state.moveSpeed * airFriction * delta;

        // Apply air friction
        m_state.velocity.x *= airControl;
        m_state.velocity.z *= airFriction;
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
    

    // Apply movement
    Vector3 relativeMove = moveToFacingDirection(deltaTime, moveDirection);

    updateVelocity(deltaTime, relativeMove);

    // Change animations
    updateAnimations(moveDirection);

    // Apply gravity
    if (!m_state.isGrounded) {
        m_state.velocity.y += m_appSettings.physicsSettings.gravity * deltaTime;
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
}

void Player::Draw() {
    // Draw player model and debug visuals
}

bool Player::Initialize() {
    if (!LoadPlayerModel(m_settings.model))
        return false;

    // Load animations
    m_animManager = std::make_unique<AnimationManager>();
    if (!m_animManager->LoadAnimations(m_settings.model)) {
        LOG_ERROR("Failed to load animations");
        return false;
    }

    return true;
}

void Player::checkCollisions(Vector3& newPosition) {


    std::pair<float, int> collisionResult = m_terrain->CheckCollision(
        newPosition, m_state.radius, m_state.height);
    float groundHeight = collisionResult.first;
    int collidingTriangleIndex = collisionResult.second;

    //bool wasGrounded = isGrounded;
    //m_isGrounded = false;

    if (collidingTriangleIndex != -1) {
        float feetHeight =
            newPosition.y - m_state.height / 2;
        if (feetHeight <= groundHeight + m_appSettings.physicsSettings.collisionGroundCheckDistance) {
            newPosition.y =
                groundHeight + m_state.height / 2;
            if (m_state.velocity.y < 0) {
                m_state.velocity.y = 0;
            }
            m_state.isGrounded = true;
            m_state.isJumping = false;
        }
    }
}

}  // namespace arena