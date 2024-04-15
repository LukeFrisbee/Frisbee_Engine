#include "fps_control.h"

#include <limits>
#include "global_data.h"

namespace fengine {
	void FPSControl::moveInPlaneXZ(GLFWwindow* glfwWindow, float dt, GameObject& gameObject)
	{
		// Rotation
		{
			glm::vec3 rotate{ 0.0f };

			double xPos;
			double yPos;
			glfwGetCursorPos(glfwWindow, &xPos, &yPos);
			double deltaX = xPos - m_lastX;
			double deltaY = yPos - m_lastY;

			rotate.x += -deltaY;
			rotate.y += deltaX;

			GlobalData::getInstance().debugString += 
				std::to_string(xPos) + " " + std::to_string(yPos) + "\n"
				+ std::to_string(m_lastX) + " " + std::to_string(m_lastY) + "\n";

			// rotate if non-zero
			if (rotate.x != 0 || rotate.y != 0) {
				gameObject.transform.rotation += m_lookSpeed * dt * glm::normalize(rotate);
			}

			// clamp rotation
			//gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
			//gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

			m_lastX = xPos;
			m_lastY = yPos;
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
				gameObject.transform.translation += m_lookSpeed * dt * glm::normalize(moveDir);
			}
		}
	}
}