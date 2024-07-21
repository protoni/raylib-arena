#include "player.h"
#include "terrain.h"
#include "utils.h"
#include "logger.h"

namespace arena {

Player::Player(const char* model) : m_modelPath(model) {
    // Initialize player properties
}

Player::~Player() {}

bool Player::LoadPlayerModel(const char* modelPath) {
    m_model = LoadModel(modelPath);
    if (m_model.meshCount == 0) {
        LOG_ERROR("Failed to load player model from path: ", modelPath);
        return false;
    }
    LOG_INFO("Successfully loaded player model");
    return true;
}

void Player::Update(float deltaTime, const std::vector<Vector3>& colliders) {
    // Handle movement, jumping, collision, etc.
    // This will contain most of the player update logic from your main loop
}

void Player::Draw() {
    // Draw player model and debug visuals
}

bool Player::Initialize() const {
    return LoadPlayerModel(m_modelPath);
}

}  // namespace arena