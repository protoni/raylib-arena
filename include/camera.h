#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "settings.h"

namespace arena {



class Camera {
   public:
    Camera(const CameraSettings& settings);
    void Update(const Vector3& targetPosition,
                const Vector3& playerFacing,
                const float deltaTime);
    void UpdateFreeLookCamera(const Vector3& targetPosition,
                              const Vector3& playerFacing,
                              const float deltaTime);
    const Camera3D& GetCamera() const { return m_camera; }
    const Vector3& GetPosition() const { return m_camera.position; }

   private:
    Camera3D m_camera = {0};
    CameraSettings m_settings;
};

}  // namespace arena

#endif  // CAMERA_H