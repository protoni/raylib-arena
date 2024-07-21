#ifndef SHADER_HANDLER_H
#define SHADER_HANDLER_H

#include "camera.h"
#include "raylib.h"

namespace arena {

#define GLSL_VERSION 330

class ShaderHandler {
   public:
    ShaderHandler(Camera* camera);
    virtual ~ShaderHandler();
    bool Load(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Begin();
    void End();
    void Update();
    const Shader& GetShader() const { return m_shader; }
    void SetCameraPosition(const Vector3& position);
    // ... other methods

   private:
    Shader m_shader;
    Camera* m_camera;

};

}  // namespace arena
#endif // SHADER_HANDLER_H