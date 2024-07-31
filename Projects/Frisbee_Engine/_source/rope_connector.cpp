#include "rope_connector.h"
#include "global_data.h"
#include "camera.h"

#include "GLFW/glfw3.h"

#include "physics/ray_cast.h"

#include <iostream>

#include "components/transform.h"

#include "logger.h"

#include "board.h"

namespace fengine {
	void RopeConnector::Start() 
	{
		{
			entt::entity e = m_ecs.create();
			m_ecs.emplace<Transform>(e);
			//m_ecs.emplace<Pin>(e, 0);
			auto render = RenderObject();
			render.shaderId = 0;
			render.modelId = 2;
			auto renderId = m_rendererResources.addRenderObject(std::move(render));
			m_ecs.emplace<PhysicsSphere>(e, 0.5f);
			m_ecs.emplace<RenderId>(e, renderId);
			const pin::Pin& pin = m_board.addPin(pin::PinType::Blue);
			m_ecs.emplace<Pin>(e, pin);
		}
		{
			auto e = m_ecs.create();
			m_ecs.emplace<Transform>(e).position = { -5.0f, 0.0f, 0.0f };
			//m_ecs.emplace<Pin>(e, 0);
			RenderObject render = RenderObject();
			render.shaderId = 0;
			render.modelId = 2;
			auto renderId = m_rendererResources.addRenderObject(std::move(render));
			m_ecs.emplace<RenderId>(e, renderId);
			m_ecs.emplace<PhysicsSphere>(e, 0.5f);
			const pin::Pin& pin = m_board.addPin(pin::PinType::Blue);
			m_ecs.emplace<Pin>(e, pin);
		}
		{
			auto e = m_ecs.create();
			m_ecs.emplace<Transform>(e).position = { 0.0f, -5.0f, 0.0f };
			//m_ecs.emplace<Pin>(e, 0);
			RenderObject render = RenderObject();
			render.shaderId = 0;
			render.modelId = 2;
			auto renderId = m_rendererResources.addRenderObject(std::move(render));
			m_rendererResources.getRenderObject(renderId).transform.translation = { 0.0f, -5.0f, 0.0f };
			m_ecs.emplace<RenderId>(e, renderId);
			m_ecs.emplace<PhysicsSphere>(e, 0.5f);
			const pin::Pin& pin = m_board.addPin(pin::PinType::Blue);
			m_ecs.emplace<Pin>(e, pin);
		}

		auto e = m_ecs.create();
		m_ecs.emplace<Transform>(e).position = { 0.0f, 0.0f, 0.0f };
		m_ecs.emplace<RenderId>(e, activeRopeID);
		m_ecs.emplace<PhysicsCapsule>(e, glm::vec3(0.0f, -5.0f, 0.0f), 0.5f);
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

		//auto entities = ps.lineCast(camPos, rayEnd);
		//if (entities.size() > 0) {
		//	LOG_DEBUG(m_ecs.get<Transform>(entities[0]).position.x);
		//}

		auto test = ps.lineCast(camPos, rayEnd);

		if (t >= 0) {
			glm::vec3 intersectionPoint = mouseWorldPos + t * direction;

			if (!hasActiveRope && m_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				if (auto entites = ps.lineCast(camPos, rayEnd);
					!entites.empty()) {
					activeRopeID = _createRope();
					m_startPos = m_ecs.get<Transform>(entites[0]).position;
					//*p_pin = m_ecs.get<Pin>(entites[0]).pin;
					_handleRope(activeRopeID, m_startPos, intersectionPoint);
					hasActiveRope = true;
				}
			}
			else if (hasActiveRope && m_input.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			{
				if (auto entities = ps.lineCast(camPos, rayEnd);
					!entities.empty()) // && p_pin != nullptr)
				{
					//pin::Pin endPin = m_ecs.get<Pin>(entities[0]).pin;
					//m_board.addRope(*p_pin, endPin);

					auto endPos = m_ecs.get<Transform>(entities[0]).position;
					_handleRope(activeRopeID, m_startPos, endPos);
				}
				else {
					m_rendererResources.removeRenderObject(activeRopeID);
				}
				hasActiveRope = false;
			}
			else if(hasActiveRope) {
				_handleRope(activeRopeID, m_startPos, intersectionPoint);
			}
		}
	}

	void RopeConnector::_handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end) const {
		float s = 0.15f;

		glm::vec3 direction = glm::normalize(end - start);
		glm::vec3 perpendicular = glm::vec3(direction.y, -direction.x, 0);

		glm::vec3 flu = start + (s * -perpendicular);
		glm::vec3 fld = start + (s * perpendicular);
		glm::vec3 fru = end + (s * -perpendicular);
		glm::vec3 frd = end + (s * perpendicular);
		// glm::vec3 blu = start + (s * -perpendicular);
		// glm::vec3 bld = start + (s * perpendicular);
		// glm::vec3 bru = end + (s * -perpendicular);
		// glm::vec3 brd = end + (s * perpendicular);

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
