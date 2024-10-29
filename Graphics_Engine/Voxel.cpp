#include "Voxel.h"

Voxel::Voxel() : m_active(false), m_type(Air), m_color(glm::vec3(0.0f)) {}

Voxel::Voxel(Type type) : m_active(true), m_type(type) {
    // Assign color based on type
    switch (type) {
    case Grass:
        m_color = glm::vec3(0.0f, 1.0f, 0.0f); // Green
        break;
    case Dirt:
        m_color = glm::vec3(0.59f, 0.29f, 0.0f); // Brown
        break;
    case Stone:
        m_color = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
        break;
    default:
        m_color = glm::vec3(1.0f); // Default to white
        break;
    }
}

bool Voxel::IsActive() const {
    return m_active;
}

void Voxel::SetActive(bool active) {
    m_active = active;
}

Voxel::Type Voxel::GetType() const {
    return m_type;
}

void Voxel::SetType(Type type) {
    m_type = type;
    m_active = (type != Air);

    // Assign color based on type
    switch (type) {
    case Grass:
        m_color = glm::vec3(0.0f, 1.0f, 0.0f); // Green
        break;
    case Dirt:
        m_color = glm::vec3(0.59f, 0.29f, 0.0f); // Brown
        break;
    case Stone:
        m_color = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
        break;
    default:
        m_color = glm::vec3(1.0f); // Default to white
        break;
    }
}

glm::vec3 Voxel::GetColor() const {
    return m_color;
}
