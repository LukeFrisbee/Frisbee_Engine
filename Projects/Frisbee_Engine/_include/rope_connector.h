#pragma once

#include "script.h"
#include "window.h"

#include "physics/physics_sphere.h"

#include "camera.h"
#include "input.h"
#include "screen.h"
#include "renderer_resources.h"

#include "physics/PhysicsSystem.h"

#include "entt/entt.hpp"
#include "board.h"

namespace fengine {
	class RopeConnector : public Script {
	public:
		RopeConnector(entt::registry& ecs, Camera& camera, Input& input, Screen& screen, RendererResources& rendererResources)
			: m_ecs(ecs), m_camera(camera), m_input(input), m_screen(screen), m_rendererResources(rendererResources) {}
		void Start() override;
		void Update() override;

	private:
		pin::Board m_board{};

		entt::registry& m_ecs;
		Camera& m_camera;
		Input& m_input;
		Screen& m_screen;
		RendererResources& m_rendererResources;

		glm::vec3 m_startPos{};
		pin::Pin* p_pin;

		void _handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end) const;
		uint32_t _createRope();

		physics::PhysicsSystem ps{m_ecs};

		bool hasActiveRope = false;
		uint32_t activeRopeID{};
	};
}