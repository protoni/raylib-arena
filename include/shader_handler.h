#ifndef SHADER_HANDLER_H
#define SHADER_HANDLER_H

#include "camera.h"
#include "settings.h"
#include "raylib.h"

namespace arena {

#define GLSL_VERSION 330

class ShaderHandler {
   public:
    ShaderHandler(const Settings& settings, Camera* camera);
    virtual ~ShaderHandler();
    bool Load(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Begin();
    void End();
    void Update();
    const Shader& GetShader() const { return m_shader; }
    void SetCameraPosition(const Vector3& position);


   private:
    Settings m_settings;
    Shader m_shader;
    Camera* m_camera;

};

}  // namespace arena
#endif // SHADER_HANDLER_H