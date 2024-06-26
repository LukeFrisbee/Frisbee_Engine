#pragma once

#include "render_object.h"

namespace fengine {
	class FPSControl {
	public:
		struct KeyMappings {
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;

			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;

			int lockCursor = GLFW_KEY_ESCAPE;
		};

		void moveInPlaneXZ(const FWindow& fWindow, float dt, RenderObject& gameObject);
		static glm::vec3 getMouseWorldPosition(const FWindow& fWindow,
			const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	private:
		KeyMappings keys{};

		float m_moveSpeed{ 3.0f };
		float m_lookSpeed{ 0.2f };

		bool m_isPressingEscape{ false };
		bool m_isLocked{ false };
	};
}