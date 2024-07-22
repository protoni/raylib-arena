#include "camera.h"
#include "logger.h"
#include "utils.h"

namespace arena {

Camera::Camera(const CameraSettings& settings) : m_settings(settings) {

    // Set initial camera position
    m_camera.position = Vector3{0.0f, 10.0f, 10.0f};

    // Camera looking at point
    m_camera.target = Vector3{0.0f, 0.0f, 0.0f};

    // Camera up vector (rotation towards target)
    m_camera.up = Vector3{0.0f, 1.0f, 0.0f};

    // Camera field-of-view Y
    m_camera.fovy = 45.0f;

    // Camera mode type
    m_camera.projection = CAMERA_PERSPECTIVE;
}

void Camera::Update(const Vector3& targetPosition, const Vector3& playerFacing,
                    const float deltaTime) {
    // Update camera
    UpdateFreeLookCamera(targetPosition, playerFacing, deltaTime);
}

void Camera::UpdateFreeLookCamera(const Vector3& targetPosition,
                                  const Vector3& playerFacing,
                                  const float deltaTime) {
    static float angleY = 0.0f;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouseDelta = GetMouseDelta();
        angleY += mouseDelta.y * m_settings.mouseSensitivity * deltaTime;

        // Clamp vertical angle to avoid camera flip
        angleY = utils::Clamp(angleY, -89.0f, 89.0f);
    }

    // Calculate camera position based on player's facing direction and vertical angle
    float horizontalDistance =
        cosf(angleY * DEG2RAD) * m_settings.cameraDistance;
    float verticalDistance = sinf(angleY * DEG2RAD) * m_settings.cameraDistance;

    Vector3 offset = {-playerFacing.x * horizontalDistance, verticalDistance,
                      -playerFacing.z * horizontalDistance};

    m_camera.position = Vector3Add(targetPosition, offset);
    m_camera.target = targetPosition;
    m_camera.up = Vector3{0.0f, 1.0f, 0.0f};
}

}  // namespace arena