#pragma once

#include "script.h"
#include "window.h"

#include "physics/physics_sphere.h"

namespace fengine {
	class RopeConnector : public Script {
	public:
		using Script::Script;
		void Start() override;
		void Update() override;
	private:

		void _handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end);
		uint32_t _createRope();

		PhysicsSphere physicsSphere{ glm::vec3{-5.0, 0.0, 0.0}, 0.5 };

		bool hasActiveRope = false;
		uint32_t activeRopeID;
	};
}