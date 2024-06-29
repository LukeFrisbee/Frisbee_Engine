#pragma once

#include "script.h"
#include "window.h"

#include "physics/physics_sphere.h"

#include "camera.h"
#include "input.h"
#include "screen.h"
#include "model_builder.h"
#include "renderer_resources.h"

namespace fengine {
	class RopeConnector : public Script {
	public:
		RopeConnector(Camera& camera, Input& input, Screen& screen, ModelBuilder& modelBuilder, RendererResources& rendererResources)
			: m_camera(camera), m_input(input), m_screen(screen), m_modelBuilder(modelBuilder), m_rendererResources(rendererResources) {};
		void Start() override;
		void Update() override;

	private:
		Camera& m_camera;
		Input& m_input;
		Screen& m_screen;
		ModelBuilder& m_modelBuilder;
		RendererResources & m_rendererResources;

		void _handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end);
		uint32_t _createRope();

		PhysicsSphere physicsSphere{ glm::vec3{-5.0, 0.0, 0.0}, 0.5 };

		bool hasActiveRope = false;
		uint32_t activeRopeID;
	};
}