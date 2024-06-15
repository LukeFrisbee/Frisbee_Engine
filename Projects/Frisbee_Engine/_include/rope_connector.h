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
		PhysicsSphere physicsSphere{ glm::vec3{-5.0, 0.0, 0.0}, 0.5 };
		void _handleRope(uint32_t ropeRenderID, glm::vec3 start, glm::vec3 end);
		uint32_t activeRopeID;
	};
}