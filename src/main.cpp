#include <iostream>
#include <utility>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "logger.h"







int lastCollidingTriangleIndex = -1;






// Function to check collision between a point and a triangle
bool CheckCollisionPointTriangle(Vector3 point, Vector3 v1, Vector3 v2,
                                 Vector3 v3) {
    // Compute vectors
    Vector3 u = Vector3Subtract(v2, v1);
    Vector3 v = Vector3Subtract(v3, v1);
    Vector3 w = Vector3Subtract(point, v1);

    // Compute dot products
    float uu = Vector3DotProduct(u, u);
    float uv = Vector3DotProduct(u, v);
    float vv = Vector3DotProduct(v, v);
    float wu = Vector3DotProduct(w, u);
    float wv = Vector3DotProduct(w, v);

    // Compute barycentric coordinates
    float denom = uv * uv - uu * vv;
    float s = (uv * wv - vv * wu) / denom;
    float t = (uv * wu - uu * wv) / denom;

    // Check if point is in triangle
    return (s >= 0.0f) && (t >= 0.0f) && (s + t <= 1.0f);
}

bool CheckCollisionSphereTriangle(Vector3 center, float radius, Vector3 v1,
                                  Vector3 v2, Vector3 v3,
                                  float& collisionHeight) {
    // Check if sphere is above the triangle's bounding box
    float minX = fmin(fmin(v1.x, v2.x), v3.x);
    float maxX = fmax(fmax(v1.x, v2.x), v3.x);
    float minZ = fmin(fmin(v1.z, v2.z), v3.z);
    float maxZ = fmax(fmax(v1.z, v2.z), v3.z);

    if (center.x < minX - radius || center.x > maxX + radius ||
        center.z < minZ - radius || center.z > maxZ + radius) {
        return false;
    }

    // Calculate the height of the triangle at the sphere's position
    Vector3 edge1 = Vector3Subtract(v2, v1);
    Vector3 edge2 = Vector3Subtract(v3, v1);
    Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

    float D = -(normal.x * v1.x + normal.y * v1.y + normal.z * v1.z);
    float triangleHeight =
        (-normal.x * center.x - normal.z * center.z - D) / normal.y;

    // Check if the sphere is colliding with the triangle
    if (center.y - radius <= triangleHeight &&
        center.y + radius >= triangleHeight) {
        collisionHeight = triangleHeight;
        return true;
    }

    return false;
}






// Function to print material info
void PrintMaterialInfo(Model model) {
    for (int i = 0; i < model.materialCount; i++) {
        Material material = model.materials[i];
        std::cout << "Material " << i << ":" << std::endl;
        std::cout << "  shader: " << material.shader.id << std::endl;
        std::cout << "  maps: " << std::endl;
        for (int j = 0; j < MAX_MATERIAL_MAPS; j++) {
            if (material.maps[j].texture.id > 0) {
                std::cout << "    Map " << j
                          << ": Texture ID: " << material.maps[j].texture.id
                          << std::endl;
            }
        }
    }
}

int FindAnimationByName(const std::vector<AnimationInfo>& animations,
                        const char* name) {
    for (size_t i = 0; i < animations.size(); i++) {
        if (strcmp(animations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;  // Animation not found
}

int main() {
    using namespace arena;

    // Initialize logger. Log to console only
    Logger::Init(LogLevel::DEBUG);

    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Create window
    InitWindow(screenWidth, screenHeight, "Raylib Arena");


    

    // Load the models
    Model playerModel = LoadModel("../assets/models/orc.glb");
    if (playerModel.meshCount == 0) {
        std::cerr << "Failed to load player model." << std::endl;
        CloseWindow();
        return -1;
    }

    Model terrainModel = LoadModel("../assets/models/map.glb");
    if (terrainModel.meshCount == 0) {
        std::cerr << "Failed to load terrain model." << std::endl;
        UnloadModel(playerModel);
        CloseWindow();
        return -1;
    }

    // Print material info for debugging
    PrintMaterialInfo(playerModel);
    PrintMaterialInfo(terrainModel);

    // Load colliders from the terrain model mesh
    std::vector<Vector3> colliders =
        LoadCollidersFromMesh(terrainModel.meshes[0]);
    if (colliders.empty()) {
        std::cerr << "Failed to load colliders from terrain model."
                  << std::endl;
        UnloadModel(playerModel);
        UnloadModel(terrainModel);
        CloseWindow();
        return -1;
    }

    // Print collider size and some elements for debugging
    std::cout << "Colliders size: " << colliders.size() << std::endl;
    for (size_t i = 0; i < colliders.size() && i < 10;
         ++i) {  // Print first 10 elements for debugging
        std::cout << "Collider " << i << ": (" << colliders[i].x << ", "
                  << colliders[i].y << ", " << colliders[i].z << ")"
                  << std::endl;
    }

    // Player position
    Vector3 playerPosition = {0.0f, 5.0f, 0.0f};
    Vector3 playerScale = {0.5f, 0.5f, 0.5f};

    std::vector<AnimationInfo> animations;
    int animCount = 0;
    ModelAnimation* animationData =
        LoadModelAnimations("../assets/models/orc.glb", &animCount);
    if (animationData == nullptr || animCount == 0) {
        std::cerr << "Failed to load animations for player model." << std::endl;
        UnloadModel(playerModel);
        UnloadModel(terrainModel);
        CloseWindow();
        return -1;
    } else {
        for (int i = 0; i < animCount; i++) {
            animations.push_back({&animationData[i], animationData[i].name});
            std::cout << "Loaded animation: " << animationData[i].name
                      << std::endl;
        }
    }

    //int currentAnimIndex = FindAnimationByName(animations, "idle");
    std::string currentAnimName = "idle";
    int currentAnim = 0;
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

        // Update
        float deltaTime = GetFrameTime();
        Vector3 oldPosition = playerPosition;

        float cameraPos[3] = {camera.position.x, camera.position.y,
                              camera.position.z};
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos,
                       SHADER_UNIFORM_VEC3);

        // Basic player movement
        Vector3 forward = Vector3Subtract(camera.target, camera.position);
        forward.y = 0;  // keep movement in the xz plane
        forward = Vector3Normalize(forward);

        Vector3 right = Vector3CrossProduct(forward, camera.up);

        //if (IsKeyDown(KEY_W)) {
        //    playerPosition = Vector3Add(playerPosition, Vector3Scale(forward, 2.0f * deltaTime));
        //    std::cout << "W pressed, moving forward" << std::endl;
        //}
        //if (IsKeyDown(KEY_S)) {
        //    playerPosition = Vector3Subtract(playerPosition, Vector3Scale(forward, 2.0f * deltaTime));
        //    std::cout << "S pressed, moving backward" << std::endl;
        //}
        //if (IsKeyDown(KEY_A)) {
        //    playerPosition = Vector3Subtract(playerPosition, Vector3Scale(right, 2.0f * deltaTime));
        //    std::cout << "A pressed, moving left" << std::endl;
        //}
        //if (IsKeyDown(KEY_D)) {
        //    playerPosition = Vector3Add(playerPosition, Vector3Scale(right, 2.0f * deltaTime));
        //    std::cout << "D pressed, moving right" << std::endl;
        //}
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mouseDelta = GetMouseDelta();
            float rotationY = mouseDelta.x * mouseSensitivity * deltaTime;
            playerFacing = Vector3RotateByAxisAngle(
                playerFacing, Vector3{0, 1, 0}, -rotationY * DEG2RAD);
            playerFacing = Vector3Normalize(playerFacing);
        }

        // Calculate movement direction
        Vector3 moveDirection = {0};
        if (IsKeyDown(KEY_W))
            moveDirection.x += 1.0f;
        if (IsKeyDown(KEY_S))
            moveDirection.x -= 1.0f;
        if (IsKeyDown(KEY_A))
            moveDirection.z -= 1.0f;
        if (IsKeyDown(KEY_D))
            moveDirection.z += 1.0f;

        // Apply movement relative to player's facing direction
        float moveSpeed = 2.0f;
        Matrix rotationMatrix =
            MatrixRotateY(-atan2f(playerFacing.z, playerFacing.x));
        Vector3 relativeMove = Vector3Transform(moveDirection, rotationMatrix);

        // Normalize movement direction
        if (Vector3Length(moveDirection) > 0) {
            float targetRotation = atan2f(relativeMove.x, relativeMove.z);
            float rotationDiff = targetRotation - playerRotation;

            // Normalize the rotation difference to [-PI, PI]
            if (rotationDiff > PI)
                rotationDiff -= 2 * PI;
            if (rotationDiff < -PI)
                rotationDiff += 2 * PI;

            // Smoothly interpolate the rotation
            playerRotation += rotationDiff * 10.0f * deltaTime;

            // Normalize the rotation to [0, 2*PI]
            while (playerRotation < 0)
                playerRotation += 2 * PI;
            while (playerRotation >= 2 * PI)
                playerRotation -= 2 * PI;
        }

        // Change animations
        if (Vector3Length(moveDirection) > 0 && !isJumping) {
            currentAnimName = "walk";
        } else if (isJumping) {
            currentAnimName = "jump_land";
        } else {
            currentAnimName = "idle";
        }

        // Apply gravity
        if (!isGrounded) {
            playerVelocity.y += gravity * deltaTime;
        }

        // Update velocity based on input
        if (isGrounded) {
            playerVelocity.x = relativeMove.x * moveSpeed;
            playerVelocity.z = relativeMove.z * moveSpeed;
        } else {
            // In air, apply reduced control
            playerVelocity.x +=
                relativeMove.x * moveSpeed * AIR_CONTROL * deltaTime;
            playerVelocity.z +=
                relativeMove.z * moveSpeed * AIR_CONTROL * deltaTime;

            // Apply air friction
            playerVelocity.x *= AIR_FRICTION;
            playerVelocity.z *= AIR_FRICTION;
        }

        Vector3 newPosition =
            Vector3Add(playerPosition, Vector3Scale(playerVelocity, deltaTime));

        // Apply gravity
        //if (!isGrounded) {
        //    velocityY += gravity * deltaTime;
        //}
        //else {
        //    velocityY = 0;
        //}
        //
        //// Update position
        ////Vector3 newPosition = playerPosition;
        //newPosition.y += velocityY * deltaTime;

        // Check collision with terrain
        std::pair<float, int> collisionResult =
            CheckCollisionWithTerrain(newPosition, playerRadius, playerHeight,
                                      colliders, lastCollidingTriangleIndex);
        float groundHeight = collisionResult.first;
        int collidingTriangleIndex = collisionResult.second;

        //bool wasGrounded = isGrounded;
        isGrounded = false;

        if (collidingTriangleIndex != -1) {
            float feetHeight =
                newPosition.y - playerHeight / 2;  // Adjust this line
            if (feetHeight <= groundHeight + GROUND_CHECK_DISTANCE) {
                newPosition.y =
                    groundHeight + playerHeight / 2;  // Adjust this line
                if (playerVelocity.y < 0) {
                    playerVelocity.y = 0;
                }
                isGrounded = true;
                isJumping = false;
            }
        }

        //lastCollidingTriangleIndex = collidingTriangleIndex;

        // Handle jumping
        if (IsKeyPressed(KEY_SPACE) && isGrounded) {
            playerVelocity.y = jumpSpeed;
            isJumping = true;
            isGrounded = false;
        }

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

    // De-Initialization
    UnloadModel(playerModel);   // Unload the player model
    UnloadModel(terrainModel);  // Unload the terrain model
    //for (unsigned int i = 0; i < animCount; i++) {
    //    UnloadModelAnimation(animationData[i]);
    //}
    for (auto& anim : animations) {
        UnloadModelAnimation(*anim.animation);
    }
    animations.clear();
    RL_FREE(animationData);

    UnloadShader(shader);
    CloseWindow();  // Close window and OpenGL context

    return 0;
}
