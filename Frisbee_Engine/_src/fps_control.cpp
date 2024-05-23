#include "fps_control.h"

#include <limits>
#include "global_data.h"

#include "imgui_impl_glfw.h"

namespace fengine {
	void FPSControl::moveInPlaneXZ(const FWindow& fWindow, float dt, GameObject& gameObject)
	{
		auto* glfwWindow = fWindow.getGLFWwindow();

		// Rotation
		{
			auto extent = fWindow.getExtent();
			auto xCenter = extent.width / 2;
			auto yCenter = extent.height / 2;

			if (glfwGetKey(glfwWindow, keys.lockCursor) == GLFW_PRESS) {
				if (!m_isPressingEscape) {
					m_isLocked = (m_isLocked ? false : true);
					glfwSetCursorPos(glfwWindow, xCenter, yCenter);
					m_isPressingEscape = true;
				}
			} else {
				m_isPressingEscape = false;
			}

			if (m_isLocked) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);

				double xPos, yPos;
				glfwGetCursorPos(glfwWindow, &xPos, &yPos);
				double deltaX = xPos - xCenter;
				double deltaY = yPos - yCenter;

				glm::vec3 rotate{ 0.0f };
				rotate.x += -deltaY;
				rotate.y += deltaX;

				// Rotate
				if (rotate.x != 0 || rotate.y != 0) {
					gameObject.transform.rotation += m_lookSpeed * dt * rotate; // *glm::normalize(rotate);
				}
				// Clamp rotation
				gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
				gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

				glfwSetCursorPos(glfwWindow, xCenter, yCenter);
			} else {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
			}
		}

		// Movement
		{
			float yaw = gameObject.transform.rotation.y;
			const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
			const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

			glm::vec3 moveDir{ 0.0f };

			if (glfwGetKey(glfwWindow, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
			if (glfwGetKey(glfwWindow, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
			if (glfwGetKey(glfwWindow, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
			if (glfwGetKey(glfwWindow, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
			if (glfwGetKey(glfwWindow, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
			if (glfwGetKey(glfwWindow, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

			// move if non-zero
			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.translation += m_moveSpeed * dt * glm::normalize(moveDir);
			}
		}
	}
}