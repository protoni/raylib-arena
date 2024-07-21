#ifndef SHADER_HANDLER_H
#define SHADER_HANDLER_H

#include "raylib.h"

namespace arena {

#define GLSL_VERSION 330

class ShaderHandler {
   public:
    bool Load(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Begin();
    void End();
    const Shader& GetShader() const { return m_shader; }
    void SetCameraPosition(const Vector3& position);
    // ... other methods

   private:
    Shader m_shader;  // raylib Shader
    // ... other properties
};

}  // namespace arena
#endif // SHADER_HANDLER_H