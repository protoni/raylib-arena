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

    LOG_DEBUG("----------------------------------------------------");
    LOG_DEBUG("Start camera position: (", m_camera.position.x, ", ",
              m_camera.position.y, ", ", m_camera.position.z, ")");
    LOG_DEBUG("Start camera target: (", m_camera.target.x, ", ",
              m_camera.target.y, ", ", m_camera.target.z, ")");

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouseDelta = GetMouseDelta();
        angleY += mouseDelta.y * m_settings.mouseSensitivity * deltaTime;

        // Clamp vertical angle to avoid camera flip
        angleY = utils::Clamp(angleY, -89.0f, 89.0f);
    }

    LOG_DEBUG("angleY: ", angleY);
    LOG_DEBUG("m_settings.cameraDistance: ", m_settings.cameraDistance);
    LOG_DEBUG("playerFacing: (", playerFacing.x, ", ", playerFacing.y, ", ",
              playerFacing.z, ")");

    // Calculate camera position based on player's facing direction and vertical angle
    float horizontalDistance =
        cosf(angleY * DEG2RAD) * m_settings.cameraDistance;
    float verticalDistance = sinf(angleY * DEG2RAD) * m_settings.cameraDistance;

    LOG_DEBUG("horizontalDistance: ", horizontalDistance);
    LOG_DEBUG("verticalDistance: ", verticalDistance);

    Vector3 offset = {-playerFacing.x * horizontalDistance, verticalDistance,
                      -playerFacing.z * horizontalDistance};

    LOG_DEBUG("offset: (", offset.x, ", ", offset.y, ", ", offset.z, ")");
    LOG_DEBUG("targetPosition: (", targetPosition.x, ", ", targetPosition.y,
              ", ", targetPosition.z, ")");

    m_camera.position = Vector3Add(targetPosition, offset);
    m_camera.target = targetPosition;
    m_camera.up = Vector3{0.0f, 1.0f, 0.0f};

    LOG_DEBUG("Final camera position: (", m_camera.position.x, ", ",
              m_camera.position.y, ", ", m_camera.position.z, ")");
    LOG_DEBUG("Final camera target: (", m_camera.target.x, ", ",
              m_camera.target.y, ", ", m_camera.target.z, ")");
    LOG_DEBUG("----------------------------------------------------");
}

}  // namespace arena