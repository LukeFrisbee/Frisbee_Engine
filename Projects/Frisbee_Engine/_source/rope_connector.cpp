#include "rope_connector.h"
#include "global_data.h"
#include "camera.h"

#include "GLFW/glfw3.h"

#include "physics/ray_cast.h"

#include <iostream>

namespace fengine {
	void RopeConnector::Start() 
	{
		
	}

	void RopeConnector::Update() 
	{
		GlobalData& globalData = GlobalData::getInstance();

		auto camPos = m_camera.getPosition();

		double xPos;
		double yPos;
		m_input.getMousePos(xPos, yPos);

		auto mouseWorldPos = m_camera.getMouseWorldPosition(xPos, yPos, m_screen.getWidth(), m_screen.getHeight());

		globalData.mousePos = mouseWorldPos;
		auto direction = glm::normalize(mouseWorldPos - camPos);
		glm::vec3 rayEnd = camPos + direction * 100.0f;

		float planeZ = 0.0f;
		float t = (planeZ - mouseWorldPos.z) / direction.z;
		// Intersects Plane
		if (t >= 0) {
			glm::vec3 intersectionPoint = mouseWorldPos + t * direction;
			if (!hasActiveRope && m_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				hasActiveRope = true;
				activeRopeID = _createRope();
				_handleRope(activeRopeID, glm::vec3{}, intersectionPoint);
			}
			else if (hasActiveRope && m_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
				m_rendererResources.removeRenderObject(activeRopeID);
				hasActiveRope = false;
			}
			else if (hasActiveRope) {
				_handleRope(activeRopeID, glm::vec3{}, intersectionPoint);
			}
		}

		RayCast raycast{};
		PhysicsShape* shape = nullptr;
		if (raycast.castLine(shape, camPos, rayEnd))
		{
			globalData.debugString += "found shape!\n";
		}
	}

	void RopeConnector::_handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end) 
	{
		float s = 0.75f / 2;

		glm::vec3 direction = glm::normalize(end - start);
		glm::vec3 perpendicular = glm::vec3(direction.y, -direction.x, 0);

		glm::vec3 flu = start + (s * -perpendicular);
		glm::vec3 fld = start + (s * perpendicular);
		glm::vec3 fru = end + (s * -perpendicular);
		glm::vec3 frd = end + (s * perpendicular);
		glm::vec3 blu = start + (s * -perpendicular);
		glm::vec3 bld = start + (s * perpendicular);
		glm::vec3 bru = end + (s * -perpendicular);
		glm::vec3 brd = end + (s * perpendicular);

		std::vector<glm::vec3> vertices = {
			fld, flu, frd, flu, fru, frd
		};

		std::vector<Model::Vertex> vertexData =
		{
			{ fld, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ flu, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ frd, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ flu, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ fru, {0.0, 0.0, 1.0}, {0.0, 0.0} },
			{ frd, {0.0, 0.0, 1.0}, {0.0, 0.0} },
		};

		auto& ropeRender = m_rendererResources.getRenderObject(ropeRenderID);
		//ropeRender.model->updateVertexBuffers(vertexData);
	}

	uint32_t RopeConnector::_createRope() {
		RenderObject rope = RenderObject();
		rope.modelId = 0; 
		rope.shaderId = 1;
		activeRopeID = m_rendererResources.addRenderObject(std::move(rope));
		return activeRopeID; 
	}
}
