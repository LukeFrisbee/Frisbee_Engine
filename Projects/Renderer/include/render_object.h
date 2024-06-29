#pragma once


#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include "model.h"
#include "shader.h"

namespace fengine {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 modelMatrix();
        glm::mat3 normalMatrix();
    };

    class RenderObject {
    public:
        RenderObject() = default;
        TransformComponent transform{};
        int modelId{};
        int shaderId{};
        std::vector<UBO*> uniforms = std::vector<UBO*>(0);

    public:
        RenderObject(const RenderObject&) = delete;
        RenderObject& operator=(const RenderObject&) = delete;
        RenderObject(RenderObject&&) = default;
        RenderObject& operator=(RenderObject&&) = default;
    };
} 