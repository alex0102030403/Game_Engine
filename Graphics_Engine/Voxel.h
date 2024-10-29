#pragma once
#include <glm.hpp>

class Voxel {
public:
    enum Type {
        Air = 0,
        Grass,
        Dirt,
        Stone,
        // Add more types as needed
    };

    Voxel();
    Voxel(Type type);

    bool IsActive() const;
    void SetActive(bool active);

    Type GetType() const;
    void SetType(Type type);

    glm::vec3 GetColor() const; // Add this function

private:
    bool m_active;
    Type m_type;
    glm::vec3 m_color; // Store color information
};
