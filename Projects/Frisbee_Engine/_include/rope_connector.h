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
	};
}