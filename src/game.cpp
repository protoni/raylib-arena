#include "game.h"
#include "logger.h"
#include "raylib.h"

namespace arena {

bool Game::Initialize() {
    Logger::Init(LogLevel::DEBUG, "ArenaGame.log");

    // Create a camera
    m_camera = std::make_unique<Camera>(m_settings.cameraSettings);

    // Load shaders
    m_shaderHandler = std::make_unique<ShaderHandler>(m_camera.get());
    if (!m_shaderHandler->Load("../assets/shaders/base_lighting.vs",
                               "../assets/shaders/lighting.fs")) {
        LOG_ERROR("Failed to load shader");
        return false;
    }

    // Load terrain
    m_terrain = std::make_unique<Terrain>(m_settings.terrainSettings);
    if (!m_terrain->Initialize()) {
        LOG_ERROR("Failed to initialize terrain");
        return false;
    }

    // Load player
    m_player = std::make_unique<Player>(m_settings.playerSettings, m_terrain.get());
    if (!m_player->Initialize()) {
        LOG_ERROR("Failed to initialize player");
        return false;
    }

    // Add lights
    m_lights[0] = CreateLight(LIGHT_POINT, Vector3{0, 10, 0}, Vector3Zero(),
                              WHITE, m_shaderHandler->GetShader());

    return true;
}

void Game::Update() {
    float deltaTime = GetFrameTime();

    // Update lighting shader
    m_shaderHandler->Update();

    m_player->Update(deltaTime, m_terrain->GetColliders());
    m_camera->Update(m_player->position, m_player->facing, deltaTime);
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(m_camera->GetCamera());
    m_terrain->Draw();
    m_player->Draw();
    // Draw other elements
    EndMode3D();

    // Draw UI elements
    EndDrawing();
}

void Game::Cleanup() {
    // Unload resources, close window
}

}  // namespace arena
