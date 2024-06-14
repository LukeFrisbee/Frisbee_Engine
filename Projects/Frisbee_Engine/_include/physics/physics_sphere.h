#pragma once

#include "physics_shape.h"

#include <glm/glm.hpp>

namespace fengine {
	class PhysicsSphere : public PhysicsShape {
	public: 
		PhysicsSphere(glm::vec3 position, float radius);
		~PhysicsSphere();

		glm::vec3 getPosition() const override { return m_position; }
		bool collisionAtPos(glm::vec3 position) override;


	private:
		glm::vec3 m_position;
		float m_radius{1.0};
	};
}