#include <iostream>
#include <utility>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "logger.h"


int lastCollidingTriangleIndex = -1;



int main() {
    using namespace arena;

    // Initialize logger. Log to console only
    Logger::Init(LogLevel::DEBUG);

    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Create window
    InitWindow(screenWidth, screenHeight, "Raylib Arena");

    std::string currentAnimName = "idle";

    float animFrameCounter = 0.0f;
    float animSpeed = 50.0f;

    const float gravity = -9.8f;
    const float jumpSpeed = 5.0f;
    float velocityY = 0.0f;
    bool isJumping = false;
    const float playerRadius = 0.5f;
    const float JUMP_GRACE_PERIOD = 0.1f;  // 100 milliseconds
    float jumpTimer = 0.0f;
    Vector3 playerFacing = {1, 0, 0};
    bool isGrounded = false;
    const float MOVEMENT_THRESHOLD = 0.001f;
    const float VELOCITY_THRESHOLD = 0.001f;
    const float GROUND_SNAP_DISTANCE = 0.01f;
    const float AIR_CONTROL = 0.3f;    // Reduced control in air
    const float AIR_FRICTION = 0.99f;  // Slight friction in air
    Vector3 playerVelocity = {0, 0, 0};
    const float GROUND_CHECK_DISTANCE = 0.1f;
    float playerRotation = 0.0f;

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second

    // Lighting setup
    Vector3 lightPosition = {0.0f, 10.0f, 0.0f};
    Color lightColor = WHITE;  // Correct type
    Vector3 lightDirection = {0.0f, -1.0f, 0.0f};

    const float playerHeight = 1.0f;

    // Main game loop
    while (!WindowShouldClose()) {  // Detect window close button or ESC key


        // Apply movement
        Vector3 movement = Vector3Subtract(newPosition, playerPosition);
        if (Vector3Length(movement) > MOVEMENT_THRESHOLD) {
            playerPosition = newPosition;
        } else if (isGrounded) {
            // If grounded and movement is small, just update Y
            playerPosition.y = newPosition.y;
        }

        lastCollidingTriangleIndex = collidingTriangleIndex;

        // Ensure the player stays on the ground when grounded
        //if (isGrounded) {
        //    playerPosition.y = groundHeight + playerHeight / 2;
        //}

        // Clamp player position to map boundaries
        newPosition.x = clamp(newPosition.x, -MAP_WIDTH / 2, MAP_WIDTH / 2);
        newPosition.z = clamp(newPosition.z, -MAP_DEPTH / 2, MAP_DEPTH / 2);

        // If we're colliding but still moving down, force position
        //if (isColliding && movement.y < 0 && jumpTimer <= 0) {
        //    playerPosition.y = newCollisionPoint.y + playerRadius;
        //    movement.y = 0;
        //}

        // Ignore very small movements

        // Calculate how much the player has moved this frame
        //Vector3 movement = Vector3Subtract(playerPosition, oldPosition);
        if (Vector3Length(movement) > MOVEMENT_THRESHOLD) {
            playerPosition = newPosition;
        } else {
            movement = Vector3Zero();
        }

        if (fabs(velocityY) < VELOCITY_THRESHOLD) {
            velocityY = 0;
        }

        // Smooth out small fluctuations in y-position
        if (fabs(playerPosition.y - groundHeight - playerRadius) < 0.01f) {
            playerPosition.y = groundHeight + playerRadius;
        }

        // Print player position for debugging
        std::cout << "Player position: (" << playerPosition.x << ", "
                  << playerPosition.y << ", " << playerPosition.z << ")"
                  << std::endl;

        // Update animation
        int currentAnimIndex =
            FindAnimationByName(animations, currentAnimName.c_str());
        if (currentAnimIndex == -1) {
            std::cerr << "Animation not found!" << std::endl;
            currentAnimIndex = 0;  // Use the first animation as a fallback
        }

        animFrameCounter += deltaTime * animSpeed;
        if (currentAnimIndex >= 0 && currentAnimIndex < animations.size()) {
            UpdateModelAnimation(playerModel,
                                 *animations[currentAnimIndex].animation,
                                 animFrameCounter);
            if (animFrameCounter >=
                animations[currentAnimIndex].animation->frameCount) {
                animFrameCounter = 0;
            }
        }

        // Update camera
        UpdateFreeLookCamera(&camera, playerPosition, playerFacing,
                             cameraDistance, mouseSensitivity, deltaTime);

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Update shader values
        Matrix viewProjection = MatrixMultiply(
            MatrixLookAt(camera.position, camera.target, camera.up),
            MatrixPerspective(camera.fovy * DEG2RAD,
                              (float)screenWidth / (float)screenHeight, 0.1f,
                              1000.0f));

        SetShaderValueMatrix(shader, shader.locs[SHADER_LOC_MATRIX_MVP],
                             viewProjection);
        SetShaderValueMatrix(shader, shader.locs[SHADER_LOC_MATRIX_MODEL],
                             MatrixIdentity());
        SetShaderValueMatrix(shader, shader.locs[SHADER_LOC_MATRIX_NORMAL],
                             MatrixTranspose(MatrixInvert(MatrixIdentity())));
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW],
                       &camera.position.x, SHADER_UNIFORM_VEC3);

        Color diffuseColor = WHITE;
        SetShaderValue(shader, shader.locs[SHADER_LOC_COLOR_DIFFUSE],
                       &diffuseColor, SHADER_UNIFORM_VEC4);

        UpdateLightValues(shader, lights[0]);

        BeginMode3D(camera);
        BeginShaderMode(shader);

        // Draw the terrain model
        //DrawModel(terrainModel, Vector3{ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
        DrawModelEx(terrainModel, Vector3Zero(), Vector3{1, 0, 0}, 0.0f,
                    Vector3One(), WHITE);

        // Draw the player model
        //DrawModelEx(playerModel, playerPosition, Vector3{ 0.0f, 1.0f, 0.0f }, 0.0f, playerScale, WHITE);
        Vector3 modelPosition = {
            playerPosition.x,
            playerPosition.y -
                playerHeight /
                    2,  // Adjust the model to sit on top of the collision box
            playerPosition.z};
        DrawModelEx(playerModel, modelPosition, Vector3{0, 1, 0},
                    playerRotation * RAD2DEG, playerScale, WHITE);

        // Draw colliders for debugging
        //for (size_t i = 0; i < colliders.size(); i += 3) {
        //    if (i + 2 < colliders.size()) { // Ensure we don't access out-of-range elements
        //        DrawTriangle3D(colliders[i], colliders[i + 1], colliders[i + 2], RED);
        //    }
        //    else {
        //        std::cerr << "Skipping drawing collider at index " << i << " due to out-of-range access." << std::endl;
        //    }
        //}

        // Draw ground height indicator only when close to the ground
        if (playerPosition.y - groundHeight < playerHeight) {
            Vector3 groundPoint = {playerPosition.x, groundHeight,
                                   playerPosition.z};
            DrawSphere(groundPoint, 0.1f, YELLOW);
        }

        // Draw colliding triangle
        if (collidingTriangleIndex != -1 &&
            collidingTriangleIndex * 3 + 2 < colliders.size()) {
            Vector3 v1 = colliders[collidingTriangleIndex * 3];
            Vector3 v2 = colliders[collidingTriangleIndex * 3 + 1];
            Vector3 v3 = colliders[collidingTriangleIndex * 3 + 2];
            DrawTriangle3D(v1, v2, v3, RED);
            DrawSphere(playerPosition, 0.1f, GRAY);
        }

        // Draw spheres to show where the lights are
        //DrawSphereEx(lights[0].position, 0.2f, 8, 8, lights[0].color);

        // Draw light source for debugging
        //DrawSphere(lightPosition, 0.2f, lightColor); // Correct type
        //DrawLine3D(lightPosition, Vector3Add(lightPosition, lightDirection), lightColor); // Correct type

        EndShaderMode();
        // Draw colliders ( faces )
        //for (size_t i = 0; i < colliders.size(); i += 3) {
        //    DrawTriangle3D(colliders[i], colliders[i + 1], colliders[i + 2], RED);
        //}

        // Draw colliders (edges only)
        for (size_t i = 0; i < colliders.size(); i += 3) {
            DrawLine3D(colliders[i], colliders[i + 1], RED);
            DrawLine3D(colliders[i + 1], colliders[i + 2], RED);
            DrawLine3D(colliders[i + 2], colliders[i], RED);
        }

        //DrawSphereWires(playerPosition, playerRadius, 8, 8, GREEN);

        // Draw player collision box
        Vector3 boxPosition = {
            playerPosition.x,
            playerPosition.y,  // Center the box on the player's position
            playerPosition.z};
        DrawCubeWires(boxPosition, playerRadius * 2, playerHeight,
                      playerRadius * 2, GREEN);

        // Draw collision point if there's a collision
        //if (CheckCollisionWithTerrain(playerPosition, playerRadius, colliders, newCollisionPoint, newCollidingTriangleIndex)) {
        //    DrawSphere(newCollisionPoint, 0.1f, YELLOW);
        //}
        //
        //if (newCollidingTriangleIndex != -1 && newCollidingTriangleIndex * 3 + 2 < colliders.size()) {
        //    Vector3 v1 = colliders[newCollidingTriangleIndex * 3];
        //    Vector3 v2 = colliders[newCollidingTriangleIndex * 3 + 1];
        //    Vector3 v3 = colliders[newCollidingTriangleIndex * 3 + 2];
        //    DrawTriangle3D(v1, v2, v3, RED);
        //}

        // Draw ground height indicator
        Vector3 groundPoint = {playerPosition.x, groundHeight,
                               playerPosition.z};
        DrawSphere(groundPoint, 0.1f, YELLOW);

        BeginShaderMode(shader);
        DrawSphereWires(lights[0].position, 0.2f, 8, 8, YELLOW);
        DrawLine3D(lights[0].position,
                   Vector3Add(lights[0].position,
                              Vector3Scale(lights[0].target, 5.0f)),
                   YELLOW);

        DrawSphereEx(lights[0].position, 0.2f, 8, 8, lights[0].color);

        EndShaderMode();

        DrawGrid(10, 1.0f);  // Draw a grid
        EndMode3D();

        DrawText("Move with WASD, Jump with SPACE, Look around with Mouse", 10,
                 10, 20, DARKGRAY);
        DrawFPS(10, 40);  // Display the FPS at coordinates (10, 40)

        //DrawText(TextFormat("Position: %.2f, %.2f, %.2f", playerPosition.x, playerPosition.y, playerPosition.z), 10, 40, 20, BLACK);
        //DrawText(TextFormat("Velocity Y: %.2f", velocityY), 10, 70, 20, BLACK);

        DrawText(
            TextFormat("Player Position: (%.2f, %.2f, %.2f)", playerPosition.x,
                       playerPosition.y, playerPosition.z),
            10, 100, 20, DARKGRAY);
        DrawText(TextFormat("Velocity Y: %.2f", velocityY), 10, 130, 20,
                 DARKGRAY);
        DrawText(TextFormat("Is Jumping: %s", isJumping ? "Yes" : "No"), 10,
                 160, 20, DARKGRAY);
        //DrawText(TextFormat("Is Colliding: %s", isColliding ? "Yes" : "No"), 10, 190, 20, DARKGRAY);
        DrawText(TextFormat("Jump Timer: %.2f", jumpTimer), 10, 190, 20,
                 DARKGRAY);
        DrawText(TextFormat("Movement: (%.3f, %.3f, %.3f)", movement.x,
                            movement.y, movement.z),
                 10, 220, 20, DARKGRAY);
        //DrawText(TextFormat("Collision Point: (%.3f, %.3f, %.3f)", newCollisionPoint.x, newCollisionPoint.y, newCollisionPoint.z), 10, 280, 20, DARKGRAY);
        DrawText(TextFormat("Colliding Triangle: %d", collidingTriangleIndex),
                 10, 250, 20, DARKGRAY);
        DrawText(
            TextFormat("Player Velocity: (%.2f, %.2f, %.2f)", playerVelocity.x,
                       playerVelocity.y, playerVelocity.z),
            10, 280, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();  // Close window and OpenGL context

    return 0;
}
