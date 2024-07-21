#include "game.h"
#include "logger.h"
#include "raylib.h"

namespace arena {

bool Game::Initialize() {
    Logger::Init(LogLevel::DEBUG, "ArenaGame.log");

    // Load shaders
    m_shaderHandler = std::make_unique<ShaderHandler>();
    if (!m_shaderHandler->Load("../assets/shaders/base_lighting.vs",
                               "../assets/shaders/lighting.fs")) {
        LOG_ERROR("Failed to load shader");
        return false;
    }

    // Create a camera
    m_camera = std::make_unique<Camera>(m_settings.cameraSettings);

    // Add lights
    m_lights[0] = CreateLight(LIGHT_POINT, Vector3{0, 10, 0}, Vector3Zero(),
                              WHITE, m_shaderHandler->GetShader());

    // Add terrain
    m_terrain = std::make_unique<Terrain>(m_settings.terrainSettings);
    if (!m_terrain->Initialize()) {
        LOG_ERROR("Failed to initialize terrain");
        return false;
    }

    // Add player
    m_player = std::make_unique<Player>(m_settings.playerSettings);
    if (!m_player->Initialize()) {
        LOG_ERROR("Failed to initialize player");
        return false;
    }
}

void Game::Update() {
    float deltaTime = GetFrameTime();
    m_player.Update(deltaTime, m_terrain.GetColliders());
    m_camera->Update(m_player->position, m_player->facing, deltaTime);
    // Update other game elements
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(m_camera.camera);
    m_terrain.Draw();
    m_player.Draw();
    // Draw other elements
    EndMode3D();

    // Draw UI elements
    EndDrawing();
}

void Game::Cleanup() {
    // Unload resources, close window
}

}  // namespace arena
