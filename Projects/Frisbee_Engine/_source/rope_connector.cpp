#include "rope_connector.h"
#include "global_data.h"
#include "camera.h"

#include "GLFW/glfw3.h"

#include "physics/ray_cast.h"

#include <iostream>

#include "components/transform.h"

#include "logger.h"

namespace fengine {
	void RopeConnector::Start() 
	{
		{
			auto e = m_ecs.create();
			m_ecs.emplace<Transform>(e);
			m_ecs.emplace<Pin>(e, 0);
			RenderObject render = RenderObject();
			render.shaderId = 0;
			render.modelId = 2;
			auto renderId = m_rendererResources.addRenderObject(std::move(render));
			m_ecs.emplace<RenderId>(e, renderId);
		}
		{
			auto e = m_ecs.create();
			m_ecs.emplace<Transform>(e).translation = { -5.0f, 0.0f, 0.0f };
			m_ecs.emplace<Pin>(e, 0);
			RenderObject render = RenderObject();
			render.shaderId = 0;
			render.modelId = 2;
			auto renderId = m_rendererResources.addRenderObject(std::move(render));
			m_ecs.emplace<RenderId>(e, renderId);
		}
		{
			auto e = m_ecs.create();
			m_ecs.emplace<Transform>(e).translation = { 0.0f, -5.0f, 0.0f };
			m_ecs.emplace<Pin>(e, 0);
			RenderObject render = RenderObject();
			render.shaderId = 0;
			render.modelId = 2;
			auto renderId = m_rendererResources.addRenderObject(std::move(render));
			m_rendererResources.getRenderObject(renderId).transform.translation = { 0.0f, -5.0f, 0.0f };
			m_ecs.emplace<RenderId>(e, renderId);
		}
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

		if (t >= 0) {
			glm::vec3 intersectionPoint = mouseWorldPos + t * direction;
			if (!hasActiveRope) {

				if (m_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					RayCast raycast{};
					auto view = m_ecs.view<Transform, Pin>();
					for (auto& entity : view) {
						auto transform = m_ecs.get<Transform>(entity);
						PhysicsSphere sphere{ transform.translation, 1.0f };
						if (raycast.castLine(&sphere, camPos, rayEnd))
						{
							LOG_DEBUG("collision!\n");
							hasActiveRope = true;
							activeRopeID = _createRope();
							_handleRope(activeRopeID, glm::vec3{}, intersectionPoint);
							break;
						}
					}
				}
			}
			else if (m_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
				m_rendererResources.removeRenderObject(activeRopeID);
				hasActiveRope = false;
			}
			else {
				_handleRope(activeRopeID, glm::vec3{}, intersectionPoint);
			}
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
		m_rendererResources.updateModel(ropeRender.modelId, vertexData);
	}

	uint32_t RopeConnector::_createRope() {
		RenderObject rope = RenderObject();

		std::vector<Model::Vertex> vertices =
		{
			{ { 0.0, 1.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
			{ { 1.0, 1.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
			{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
			{ { 1.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
			{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} },
			{ { 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0}, {0.0, 0.0} }
		};
		rope.shaderId = 1;
		rope.modelId = m_rendererResources.createModel(vertices);
		activeRopeID = m_rendererResources.addRenderObject(std::move(rope));
		return activeRopeID; 
	}
}
