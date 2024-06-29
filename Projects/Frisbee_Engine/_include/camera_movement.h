#pragma once

#include "script.h"
#include "render_object.h"

#include "input.h"
#include "screen.h"
#include "camera.h"

namespace fengine {
	class CameraMovement : public Script {
	public:
		CameraMovement(Camera& camera, Input& input, Screen& screen)
			: m_camera(camera), m_input(input), m_screen(screen) {};
		void Update() override;
		void Start() override;


	private:
		void _move();

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
		KeyMappings keys{};

		float m_moveSpeed{ 3.0f };
		float m_lookSpeed{ 0.2f };

		bool m_isPressingEscape{ false };
		bool m_isLocked{ false };

		TransformComponent m_transform;
		Input& m_input;
		Screen& m_screen;
		Camera& m_camera;
	};
}