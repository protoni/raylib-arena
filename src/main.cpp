#include "game.h"
#include "logger.h"
#include "settings.h"

#include "raylib.h"

int main() {
    using namespace arena;
    Game game;

    // Initialize logger. Log to console only
    Logger::Init(LogLevel::DEBUG);

    // Create a window
    WindowSettings windowSettings;
    InitWindow(windowSettings.screenWidth, windowSettings.screenHeight,
               windowSettings.screenName.c_str());

    if (!game.Initialize()) {
        LOG_ERROR("Failed to initialize the game");
        return -1;
    }

    // Set our game to run at 60 frames-per-second
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {  // Detect window close button or ESC key
        game.Update();
        game.Draw();
    }

    game.Cleanup();

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}
