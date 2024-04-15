#pragma once

#include "game_object.h"

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
		};

		void moveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject);

	private:
		KeyMappings keys{};
		float m_moveSpeed{ 3.0f };
		float m_lookSpeed{ 1.5f };

		double m_lastX{};
		double m_lastY{};
	};
}