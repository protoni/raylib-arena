#include "game.h"
#include "debug.h"
#include "logger.h"
#include "raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

namespace arena {

bool Game::Initialize() {

    // Create a camera
    m_camera = std::make_unique<Camera>(m_settings.cameraSettings);

    // Load shaders
    m_shaderHandler =
        std::make_unique<ShaderHandler>(m_settings, m_camera.get());
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
    m_player = std::make_unique<Player>(m_settings, m_terrain.get());
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
    m_camera->Update(m_player->GetState().position,
                     m_player->GetState().facingDirection, deltaTime);

    UpdateLightValues(m_shaderHandler->GetShader(), m_lights[0]);

    LOG_DEBUG("Camera position: ");
    debug::PrintVec3(m_camera->GetCamera().position);
    LOG_DEBUG("Camera target: ");
    debug::PrintVec3(m_camera->GetCamera().target);
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(m_camera->GetCamera());

    m_terrain->Draw();
    m_player->Draw();

    m_shaderHandler->Update();
    m_shaderHandler->Begin();

    m_terrain->Draw();
    m_player->Draw();

    m_shaderHandler->End();

    // Draw debug stuff
    m_terrain->DrawColliderEdges();
    m_player->DrawCollisionBox();
    m_player->DrawGroundHeightIndicator();

    // Draw lights
    m_shaderHandler->Begin();
    DrawLights();
    m_shaderHandler->End();

    // Draw other elements
    DrawGrid(10, 1.0f);
    EndMode3D();

    // Draw UI elements
    DrawDebugUi();

    EndDrawing();
}

void Game::DrawDebugUi() const {
    DrawText("Move with WASD, Jump with SPACE, Look around with Mouse", 10, 10,
             20, DARKGRAY);
    DrawFPS(10, 40);  // Display the FPS at coordinates (10, 40)

    //DrawText(TextFormat("Position: %.2f, %.2f, %.2f", playerPosition.x, playerPosition.y, playerPosition.z), 10, 40, 20, BLACK);
    //DrawText(TextFormat("Velocity Y: %.2f", velocityY), 10, 70, 20, BLACK);

    PlayerState playerState = m_player->GetState();
    Vector3 playerPosition = playerState.position;
    DrawText(TextFormat("Player Position: (%.2f, %.2f, %.2f)", playerPosition.x,
                        playerPosition.y, playerPosition.z),
             10, 100, 20, DARKGRAY);
    DrawText(TextFormat("Velocity Y: %.2f", m_player->GetState().velocity.y),
             10, 130, 20, DARKGRAY);
    DrawText(TextFormat("Is Jumping: %s", playerState.isJumping ? "Yes" : "No"),
             10, 160, 20, DARKGRAY);
    //DrawText(TextFormat("Is Colliding: %s", isColliding ? "Yes" : "No"), 10, 190, 20, DARKGRAY);
    //DrawText(TextFormat("Jump Timer: %.2f", jumpTimer), 10, 190, 20, DARKGRAY);
    DrawText(TextFormat("Movement: (%.3f, %.3f, %.3f)", playerState.movement.x,
                        playerState.movement.y, playerState.movement.z),
             10, 190, 20, DARKGRAY);
    //DrawText(TextFormat("Collision Point: (%.3f, %.3f, %.3f)", newCollisionPoint.x, newCollisionPoint.y, newCollisionPoint.z), 10, 280, 20, DARKGRAY);
    DrawText(TextFormat("Colliding Triangle: %d",
                        playerState.collidingTriangleIndex),
             10, 220, 20, DARKGRAY);
    DrawText(TextFormat("Player Velocity: (%.2f, %.2f, %.2f)",
                        playerState.velocity.x, playerState.velocity.y,
                        playerState.velocity.z),
             10, 250, 20, DARKGRAY);
}

void Game::DrawLights() const {
    DrawSphereWires(m_lights[0].position, 0.2f, 8, 8, YELLOW);
    DrawLine3D(m_lights[0].position,
               Vector3Add(m_lights[0].position,
                          Vector3Scale(m_lights[0].target, 5.0f)),
               YELLOW);

    DrawSphereEx(m_lights[0].position, 0.2f, 8, 8, m_lights[0].color);
}

void Game::Cleanup() {
    // Unload resources, close window
}

}  // namespace arena
