#include "physics/physics_sphere.h"
#include "physics/physics_pool.h"

#include <math.h>

namespace fengine {
	PhysicsSphere::PhysicsSphere(glm::vec3 position, float radius) 
		: m_position{ position } , m_radius{ radius }
	{
		PhysicsPool::getinstance().PhysicsShapes.push_back(this);
	}

	PhysicsSphere::~PhysicsSphere()
	{
		auto& physicsShapes = PhysicsPool::getinstance().PhysicsShapes;
		auto me = std::find(physicsShapes.begin(), physicsShapes.end(), this);
		PhysicsPool::getinstance().PhysicsShapes.erase(me);
	}

	bool PhysicsSphere::collisionAtPos(glm::vec3 position)
	{
		auto distance = glm::length(m_position - position);
		if (distance - m_radius <= 0)
			return true;

		return false;
	}
}