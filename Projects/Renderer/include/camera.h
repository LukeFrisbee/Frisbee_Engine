#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include "window.h"

namespace fengine {
	class Camera {
		public: 
			void setOrthographicProjection(float left, float right, 
				float top, float bottom, float near, float far);
			
			void setPerspectiveProject(float fovy, float aspect, float near, float far);

			void setViewDirection(glm::vec3 position, 
				glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));

			void setViewTarget(glm::vec3 position, 
				glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));

			void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

			void setPosition(glm::vec3 position) { m_position = position; }

			glm::vec3 getMouseWorldPosition(double xPos, double yPos, int screenWidth, int screenHeight);

			const glm::vec3& getPosition() const { return m_position; }
			const glm::mat4& getProjection() const { return m_projectionMatrix; }
			const glm::mat4& getView() const { return m_viewMatrix; }

		private:
			glm::vec3 m_position{};
			glm::mat4 m_projectionMatrix { 1.0f };
			glm::mat4 m_viewMatrix { 1.0f };
	};
}