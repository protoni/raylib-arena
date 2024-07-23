#include "shader_handler.h"
#include "logger.h"

namespace arena {

ShaderHandler::ShaderHandler(const Settings& settings, Camera* camera)
    : m_camera(camera), m_settings(settings) {}

ShaderHandler::~ShaderHandler() {
    UnloadShader(m_shader);
}

bool ShaderHandler::Load(const char* vertexShaderPath,
                         const char* fragmentShaderPath) {
    // Load basic lighting shader
    m_shader = LoadShader(TextFormat(vertexShaderPath, GLSL_VERSION),
                          TextFormat(fragmentShaderPath, GLSL_VERSION));

    // Check if shader loaded successfully
    if (!IsShaderReady(m_shader)) {
        LOG_ERROR("Failed to load shader");
        return false;
    }

    LOG_INFO("Shader loaded successfully");

    // Initialize shader locations
    m_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(m_shader, "mvp");
    m_shader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocation(m_shader, "matModel");
    m_shader.locs[SHADER_LOC_MATRIX_NORMAL] =
        GetShaderLocation(m_shader, "matNormal");
    m_shader.locs[SHADER_LOC_VECTOR_VIEW] =
        GetShaderLocation(m_shader, "viewPos");
    m_shader.locs[SHADER_LOC_COLOR_DIFFUSE] =
        GetShaderLocation(m_shader, "colDiffuse");

    if (m_shader.locs[SHADER_LOC_MATRIX_MVP] == -1 ||
        m_shader.locs[SHADER_LOC_MATRIX_MODEL] == -1 ||
        m_shader.locs[SHADER_LOC_MATRIX_NORMAL] == -1 ||
        m_shader.locs[SHADER_LOC_VECTOR_VIEW] == -1 ||
        m_shader.locs[SHADER_LOC_COLOR_DIFFUSE] == -1) {
        LOG_WARNING("Some shader uniforms were not found");
    }

    int ambientLoc = GetShaderLocation(m_shader, "ambient");

    float ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    SetShaderValue(m_shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

    return true;
}

void ShaderHandler::Begin() {
    BeginShaderMode(m_shader);
}

void ShaderHandler::End() {
    EndShaderMode();
}

void ShaderHandler::Update() {
    Vector3 pos = m_camera->GetPosition();
    float cameraPos[3] = {pos.x, pos.y, pos.z};
    SetShaderValue(m_shader, m_shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                   SHADER_UNIFORM_VEC3);

    // Update shader values
    Matrix viewProjection = MatrixMultiply(
        MatrixLookAt(m_camera->GetCamera().position,
                     m_camera->GetCamera().target, m_camera->GetCamera().up),
        MatrixPerspective(m_camera->GetCamera().fovy * DEG2RAD,
                          (float)m_settings.windowSettings.screenWidth /
                              (float)m_settings.windowSettings.screenHeight,
                          0.1f, 1000.0f));

    SetShaderValueMatrix(m_shader, m_shader.locs[SHADER_LOC_MATRIX_MVP],
                         viewProjection);
    SetShaderValueMatrix(m_shader, m_shader.locs[SHADER_LOC_MATRIX_MODEL],
                         MatrixIdentity());
    SetShaderValueMatrix(m_shader, m_shader.locs[SHADER_LOC_MATRIX_NORMAL],
                         MatrixTranspose(MatrixInvert(MatrixIdentity())));
    SetShaderValue(m_shader, m_shader.locs[SHADER_LOC_VECTOR_VIEW],
                   &m_camera->GetCamera().position.x, SHADER_UNIFORM_VEC3);

    Color diffuseColor = WHITE;
    SetShaderValue(m_shader, m_shader.locs[SHADER_LOC_COLOR_DIFFUSE],
                   &diffuseColor, SHADER_UNIFORM_VEC4);

    //LOG_DEBUG(
    //    "Shader MVP Matrix: %s",
    //    TextFormat("%.2f %.2f %.2f %.2f", viewProjection.m0, viewProjection.m1,
    //               viewProjection.m2, viewProjection.m3));
}

void ShaderHandler::SetCameraPosition(const Vector3& position) {}

}  // namespace arena