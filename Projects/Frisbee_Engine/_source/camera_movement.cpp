#include "camera_movement.h"

#include <limits>
#include "global_data.h"

#include "imgui_impl_glfw.h"
#include "loop_time.h"

namespace fengine {

	void CameraMovement::Start() {
		m_transform.translation = glm::vec3(0.0f, -2.0f, -5.0f);
	}

	void CameraMovement::Update() {
		_move();
	}
 
	void CameraMovement::_move()
	{
		// Rotation
		{
			auto xCenter = m_screen.getWidth() / 2;
			auto yCenter = m_screen.getHeight() / 2;

			if (m_input.getKey(keys.lockCursor) == GLFW_PRESS) {
				if (!m_isPressingEscape) {
					m_isLocked = (m_isLocked ? false : true);
					m_input.setCursorPos(xCenter, yCenter);
					m_isPressingEscape = true;
				}
			}
			else {
				m_isPressingEscape = false;
			}

			if (m_isLocked) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);

				double xPos, yPos;
				m_input.getMousePos(xPos, yPos);
				double deltaX = xPos - xCenter;
				double deltaY = yPos - yCenter;

				glm::vec3 rotate{ 0.0f };
				rotate.x += -deltaY;
				rotate.y += deltaX;

				// Rotate
				if (rotate.x != 0 || rotate.y != 0) {
					m_transform.rotation += m_lookSpeed * LoopTime::getDeltaTime() * rotate; // *glm::normalize(rotate);
				}
				// Clamp rotation
				m_transform.rotation.x = glm::clamp(m_transform.rotation.x, -1.5f, 1.5f);
				m_transform.rotation.y = glm::mod(m_transform.rotation.y, glm::two_pi<float>());

				m_input.setCursorPos(xCenter, yCenter);
			}
			else {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
			}
		}

		// Movement
		{
			float yaw = m_transform.rotation.y;
			const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
			const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

			glm::vec3 moveDir{ 0.0f };

			if (m_input.getKey(keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
			if (m_input.getKey(keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
			if (m_input.getKey(keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
			if (m_input.getKey(keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
			if (m_input.getKey(keys.moveUp) == GLFW_PRESS) moveDir += upDir;
			if (m_input.getKey(keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

			// move if non-zero
			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				auto deltaTime = LoopTime::getDeltaTime();
				m_transform.translation += m_moveSpeed * LoopTime::getDeltaTime() * glm::normalize(moveDir);
			}
		}

		m_camera.setViewYXZ(m_transform.translation, m_transform.rotation);
		m_camera.setPosition(m_transform.translation);
	}
}