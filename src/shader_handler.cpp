#include "shader_handler.h"
#include "logger.h"

namespace arena {


bool ShaderHandler::Load(const char* vertexShaderPath,
                         const char* fragmentShaderPath) {
    // Load basic lighting shader
    Shader shader = LoadShader(TextFormat(vertexShaderPath, GLSL_VERSION),
                               TextFormat(fragmentShaderPath, GLSL_VERSION));

    // Check if shader loaded successfully
    if (!IsShaderReady(shader)) {
        const char* error = GetShaderLoaderLog();
        if (error != nullptr && strlen(error) > 0) {
            LOG_ERROR("Failed to load shader. Error: ", error);
        } else {
            LOG_ERROR("Failed to load shader. Unknown error.");
        }
        return false;
    }

    LOG_INFO("Shader loaded successfully");

    // Initialize shader locations
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(shader, "matNormal");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(shader, "colDiffuse");

    if (shader.locs[SHADER_LOC_MATRIX_MVP] == -1 ||
        shader.locs[SHADER_LOC_MATRIX_MODEL] == -1 ||
        shader.locs[SHADER_LOC_MATRIX_NORMAL] == -1 ||
        shader.locs[SHADER_LOC_VECTOR_VIEW] == -1 ||
        shader.locs[SHADER_LOC_COLOR_DIFFUSE] == -1) {
        LOG_WARNING("Some shader uniforms were not found");
    }

    int ambientLoc = GetShaderLocation(shader, "ambient");

    float ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

    return true;
}

void ShaderHandler::Begin() {}

void ShaderHandler::End() {}

void ShaderHandler::SetCameraPosition(const Vector3& position) {}

}  // namespace arena