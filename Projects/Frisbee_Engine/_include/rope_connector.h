#pragma once

#include "script.h"
#include "window.h"

#include "physics/physics_sphere.h"

#include "camera.h"
#include "input.h"
#include "screen.h"
#include "renderer_resources.h"

#include "entt/entt.hpp"

namespace fengine {
	class RopeConnector : public Script {
	public:
		RopeConnector(entt::registry& ecs, Camera& camera, Input& input, Screen& screen, RendererResources& rendererResources)
			: m_ecs(ecs), m_camera(camera), m_input(input), m_screen(screen), m_rendererResources(rendererResources) {}
		void Start() override;
		void Update() override;

	private:
		entt::registry& m_ecs;
		Camera& m_camera;
		Input& m_input;
		Screen& m_screen;
		RendererResources& m_rendererResources;

		void _handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end);
		uint32_t _createRope();

		PhysicsSphere physicsSphere{ glm::vec3{-5.0, 0.0, 0.0}, 0.5 };

		bool hasActiveRope = false;
		uint32_t activeRopeID;
	};
}